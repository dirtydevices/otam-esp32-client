#include "Adapter.h"

#include <HTTPClient.h>
#include <Preferences.h>
#include <string.h>

OTAM::Adapter::Adapter(const OtamConfig& config)
    : _config(config),
      _ready(false),
      _guid(""),
      _deviceId(""),
      _deviceProfileId(-1),
      _availableFirmware({0, 0, "", "", 0}),
      _firmwareAvailable(false),
      _availableFirmwareURL("") {}

OTAM::Adapter::~Adapter() {
    // Clean up the available firmware data if allocated
    if (_availableFirmwareCompressedData) {
        delete[] _availableFirmwareCompressedData;
        _availableFirmwareCompressedData = nullptr;
        _availableFirmwareCompressedSize = 0;
    }
}

int8_t OTAM::Adapter::init() {
    Preferences preferences;

    if (!preferences.begin("otam-store", false)) {
        return -1;  // Error: Failed to initialize NVS in readDeviceGuidFromStore
    }

    // check the store to see if we have a device guid
    if (preferences.isKey("device_guid")) {
        _guid = preferences.getString("device_guid");
    }

    if (preferences.isKey("device_id")) {
        _deviceId = preferences.getString("device_id");
    }

    if (preferences.isKey("device_profile_id")) {
        _deviceProfileId = preferences.getInt("device_profile_id");
    }

    // If we don't have a guid or our deviceId or deviceProfileId has changed, we need to initialize the device with OTAM
    if (_guid == "" || _deviceId != _config.deviceId || _deviceProfileId != _config.deviceProfileId) {
        _deviceId = _config.deviceId;
        _deviceProfileId = _config.deviceProfileId;

        if (initializeDevice() < 0) {
            return -2;  // Error: Failed to initialize device with OTAM server
        }

        // Only write to preferences if initialization or update was needed
        if (!preferences.putString("device_guid", _guid)) {
            return -3;  // Error: Failed to write device guid to store
        }

        if (!preferences.putString("device_id", _deviceId)) {
            return -4;  // Error: Failed to write device id to store
        }

        if (!preferences.putInt("device_profile_id", _deviceProfileId)) {
            return -5;  // Error: Failed to write device profile id to store
        }
    }

    preferences.end();

    _ready = true;

    return 0;  // Success
}

int8_t OTAM::Adapter::updateAvailable(bool& available) {
    // build the status URL
    char statusURL[256];
    snprintf(statusURL, sizeof(statusURL), "%s/devices/%s/status", _config.url.c_str(), _guid.c_str());

    String response;
    int8_t result = httpGet(String(statusURL), response);

    if (result != 0) {
        return -1;  // Error: Failed to get device status
    }

    String deviceStatus = "";

    if (extractString(response, "deviceStatus", deviceStatus)) {
        _firmwareAvailable = true;
        available = true;

        int fileId = 0;
        int id = 0;
        String name = "";
        String version = "";
        int size = 0;

        if (extractInt(response, "firmwareFileId", fileId) && extractInt(response, "firmwareId", id) &&
            extractString(response, "firmwareName", name) && extractString(response, "firmwareVersion", version) &&
            extractInt(response, "firmwareSize", size)) {
            _availableFirmware.fileId = fileId;
            _availableFirmware.id = id;
            _availableFirmware.name = name;
            _availableFirmware.version = version;
            _availableFirmware.size = size;

            return 0;  // Firmware update available
        } else {
            return -2;  // Error: Failed to parse firmware details
        }
    } else {
        _firmwareAvailable = false;
        available = false;

        return 1;  // No firmware update available
    }
}

int8_t OTAM::Adapter::logError(String error) {
    if (!_firmwareAvailable) {
        return -1;  // No firmware available to log error for
    }

    // build the status URL
    char statusURL[256];
    snprintf(statusURL, sizeof(statusURL), "%s/devices/%s/status", _config.url.c_str(), _guid.c_str());

    // construct the request payload
    char payload[512];
    char errorEscaped[512];
    json_escape(error.c_str(), errorEscaped, sizeof(errorEscaped));

    snprintf(payload, sizeof(payload),
             "{\"deviceStatus\":\"%s\",\"firmwareFileId\":\"%d\",\"firmwareId\":\"%d\",\"firmwareVersion\":\"%s\",\"logMessage\":"
             "\"%s\"}",
             "UPDATE_FAILED", _availableFirmware.fileId, _availableFirmware.id, _availableFirmware.version.c_str(), errorEscaped);

    return httpPost(String(statusURL), String(payload));
}

int8_t OTAM::Adapter::logMessage(String message) {
    // build the log URL
    char logURL[256];
    snprintf(logURL, sizeof(logURL), "%s/devices/%s/log", _config.url.c_str(), _guid.c_str());

    // construct the request payload
    char payload[512];
    char messageEscaped[512];
    json_escape(message.c_str(), messageEscaped, sizeof(messageEscaped));
    snprintf(payload, sizeof(payload), "{\"message\":\"%s\"}", messageEscaped);

    return httpPost(String(logURL), String(payload));
}

int8_t OTAM::Adapter::initializeDevice() {
    // build the init URL
    char initURL[256];
    snprintf(initURL, sizeof(initURL), "%s/init-device", _config.url.c_str());

    // construct the request payload
    char payload[512];
    snprintf(payload, sizeof(payload), "{\"deviceId\":\"%s\",\"deviceGuid\":\"%s\",\"deviceProfileId\":%d}", _deviceId.c_str(),
             _guid.c_str(), _deviceProfileId);

    String response;
    int8_t result = httpPost(String(initURL), String(payload), &response);

    if (result == 0) {
        _guid = response;

        return 0;  // Successful initialization
    } else {
        return -1;  // Initialization failed
    }
}

int8_t OTAM::Adapter::fetchDownloadURL() {
    // build the download URL
    char fetchDownloadURL_URL[256];
    snprintf(fetchDownloadURL_URL, sizeof(fetchDownloadURL_URL), "%s/devices/%s/firmware-file-url", _config.url.c_str(),
             _guid.c_str());

    String response;
    int8_t result = httpGet(String(fetchDownloadURL_URL), response);

    if (result == 0) {
        _availableFirmwareURL = response;

        // Successfully fetched download URL
        return 0;
    } else {
        return -1;  // Failed to fetch download URL
    }
}

int8_t OTAM::Adapter::downloadAvailableFirmware() {
    if (!_firmwareAvailable)
        return -1;  // No firmware available

    if (_availableFirmwareURL.isEmpty()) {
        int8_t result = fetchDownloadURL();
        if (result < 0)
            return result;  // Error fetching download URL
    }

    // Clean up any previous download
    if (_availableFirmwareCompressedData) {
        delete[] _availableFirmwareCompressedData;
        _availableFirmwareCompressedData = nullptr;
    }
    _availableFirmwareCompressedSize = 0;

    HTTPClient http;
    http.begin(_availableFirmwareURL);
    http.addHeader("x-api-key", _config.apiKey);

    int httpCode = http.GET();

    if (httpCode == 200) {
        size_t contentLength = http.getSize();
        if (contentLength == 0) {
            http.end();
            return -3;  // Invalid content length
        }

        // Check available heap at runtime
        size_t freeHeap = ESP.getFreeHeap();
        if (contentLength > freeHeap - (64 * 1024)) {  // Leave a margin for stack, WiFi, etc.
            http.end();
            return -5;  // Not enough RAM
        }

        _availableFirmwareCompressedSize = contentLength;
        _availableFirmwareCompressedData = new uint8_t[_availableFirmwareCompressedSize];
        if (!_availableFirmwareCompressedData) {
            _availableFirmwareCompressedSize = 0;
            http.end();
            return -6;  // Allocation failure
        }

        WiFiClient* stream = http.getStreamPtr();
        size_t bytesRead = 0;
        const size_t CHUNK = 1024;
        uint32_t lastPercent = 0;
        while (bytesRead < contentLength) {
            size_t toRead = min(CHUNK, contentLength - bytesRead);
            int n = stream->read(_availableFirmwareCompressedData + bytesRead, toRead);
            if (n <= 0) {
                delete[] _availableFirmwareCompressedData;
                _availableFirmwareCompressedData = nullptr;
                _availableFirmwareCompressedSize = 0;
                http.end();
                return -4;  // Download interrupted or failed
            }
            bytesRead += n;
            uint32_t percent = (100 * bytesRead) / contentLength;
            if (percent != lastPercent) {
                lastPercent = percent;

                emit<AdapterEvent::DownloadProgress>(percent);
            }
        }

        http.end();
        return 0;  // Success!
    } else {
        http.end();
        return -2;  // HTTP error
    }
}

int8_t OTAM::Adapter::httpPost(const String& url, const String& payload, String* response) {
    HTTPClient http;

    http.begin(url);
    http.addHeader("x-api-key", _config.apiKey);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);

    if (httpCode == 200) {
        if (response) {
            *response = http.getString();
        }

        http.end();

        return 0;  // Successful post
    } else {
        http.end();

        return -1;  // Post failed
    }
}

int8_t OTAM::Adapter::httpGet(const String& url, String& response) {
    HTTPClient http;

    http.begin(url);
    http.addHeader("x-api-key", _config.apiKey);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode == 200) {
        String tempResponse = http.getString();
        if (tempResponse.length() == 0) {
            http.end();

            return -2;  // Empty or invalid response
        }

        response = tempResponse;

        http.end();

        return 0;  // Successful get
    } else {
        http.end();

        return -1;  // Get failed
    }
}

int8_t OTAM::Adapter::clearAvailableFirmware() {
    if (_availableFirmwareCompressedData) {
        delete[] _availableFirmwareCompressedData;
        _availableFirmwareCompressedData = nullptr;
        _availableFirmwareCompressedSize = 0;
    }
    _firmwareAvailable = false;
    return 0;  // Success
}

void OTAM::Adapter::json_escape(const char* input, char* output, size_t output_size) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; ++i) {
        if (j >= output_size - 2)
            break;  // always leave room for null terminator
        switch (input[i]) {
            case '\"':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = '\"';
                }
                break;
            case '\\':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = '\\';
                }
                break;
            case '\b':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = 'b';
                }
                break;
            case '\f':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = 'f';
                }
                break;
            case '\n':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = 'n';
                }
                break;
            case '\r':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = 'r';
                }
                break;
            case '\t':
                if (j < output_size - 2) {
                    output[j++] = '\\';
                    output[j++] = 't';
                }
                break;
            default:
                output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

bool OTAM::Adapter::extractInt(const String& src, const char* key, int& out) {
    String pattern = String("\"") + key + "\":";
    int idx = src.indexOf(pattern);
    if (idx == -1)
        return false;
    idx += pattern.length();
    // Skip whitespace
    while (idx < src.length() && (src[idx] == ' ' || src[idx] == '\t'))
        idx++;
    int endIdx = idx;
    while (endIdx < src.length() && src[endIdx] >= '0' && src[endIdx] <= '9')
        endIdx++;
    if (endIdx == idx)
        return false;
    out = src.substring(idx, endIdx).toInt();
    return true;
};

bool OTAM::Adapter::extractString(const String& src, const char* key, String& out) {
    String pattern = String("\"") + key + "\":";
    int idx = src.indexOf(pattern);
    if (idx == -1)
        return false;
    idx += pattern.length();
    // Skip whitespace
    while (idx < src.length() && (src[idx] == ' ' || src[idx] == '\t'))
        idx++;
    if (idx >= src.length() || src[idx] != '"')
        return false;
    idx++;
    int endIdx = src.indexOf('"', idx);
    if (endIdx == -1)
        return false;
    out = src.substring(idx, endIdx);
    return true;
};