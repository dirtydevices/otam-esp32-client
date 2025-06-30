#include "internal/OtamUpdater.h"

// Add PROGMEM string constants at the top of the file after includes
const char ERROR_WRITE[] PROGMEM = " - Write error occurred.";
const char ERROR_ERASE[] PROGMEM = " - Erase error.";
const char ERROR_READ[] PROGMEM = " - Read error occurred.";
const char ERROR_SPACE[] PROGMEM = " - Insufficient space.";
const char ERROR_SIZE[] PROGMEM = " - Firmware size mismatch.";
const char ERROR_STREAM[] PROGMEM = " - Stream read timeout.";
const char ERROR_MD5[] PROGMEM = " - MD5 check failed.";
const char ERROR_MAGIC_BYTE[] PROGMEM = " - Magic byte mismatch.";
const char ERROR_NO_PARTITION[] PROGMEM = " - No partition available.";
const char ERROR_BAD_ARGUMENT[] PROGMEM = " - Invalid argument.";
const char ERROR_UNKNOWN[] PROGMEM = " - Unknown error.";
const char ERROR_OTA_FAILED[] PROGMEM = "OTA failed. Error #: ";
const char ERROR_NOT_ENOUGH_SPACE[] PROGMEM = "Not enough space to begin OTA.";

void OtamUpdater::onOtaAfterDownload(CallbackType afterDownloadCallback) {
    otaAfterDownloadCallback = afterDownloadCallback;
}

void OtamUpdater::onOtaDownloadProgress(NumberCallbackType progressCallback) {
    otaDownloadProgressCallback = progressCallback;
}

void OtamUpdater::onOtaSuccess(CallbackType successCallback) {
    otaSuccessCallback = successCallback;
}

void OtamUpdater::onOtaError(StringCallbackType errorCallback) {
    otaErrorCallback = errorCallback;
}

void OtamUpdater::runESP32Update(HTTPClient& http) {
    int contentLength = http.getSize();  // Get the firmware size
    // Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
    // Serial.printf("Total heap: %u\n", ESP.getHeapSize());
    // Serial.printf("Free PSRAM: %u\n", ESP.getFreePsram());
    Serial.println("Starting OTA Update...");
    // Serial.println("Content Length: " + String(contentLength));

    if (contentLength <= 0) {
        Serial.println("Invalid content length: " + String(contentLength));
        // otaErrorCallback("Content length is invalid");
        return;
    }

    bool canBegin = Update.begin(contentLength);  // Initialize OTA process
    if (canBegin) {
        // Serial.println("OTA Update initialized successfully.");
        WiFiClient* client = http.getStreamPtr();  // Get the client stream

        // Progress callback for logging the progress
        Update.onProgress([this](size_t progress, size_t total) {
            // Serial.printf("OTA Progress: %u of %u bytes\r\n", progress, total);
            otaDownloadProgressCallback(progress / (total / 100));  // Callback with percentage
        });

        // Write firmware data to flash
        size_t written = Update.writeStream(*client);
        Serial.println("Bytes written to flash: " + String(written));

        if (written != contentLength) {
            Serial.println("Warning: Written bytes do not match content length.");
        }

        if (Update.end()) {
            // Download complete
            otaAfterDownloadCallback();
            if (Update.isFinished()) {
                Serial.println("OTA Update finished successfully.");
                otaSuccessCallback();  // Call success callback
            } else {
                Serial.println("OTA Update failed to complete.");
                otaErrorCallback("OTA Update did not finish, something went wrong!");
            }
        } else {
            // Log detailed error message
            char errorMessage[128];
            strcpy(errorMessage, ERROR_OTA_FAILED);
            char errorNum[8];
            itoa(Update.getError(), errorNum, 10);
            strcat(errorMessage, errorNum);

            // Print to Serial for more debugging info
            Serial.print("Detailed Error: ");
            Update.printError(Serial);  // Detailed error output

            // Manually create human-readable error messages
            switch (Update.getError()) {
                case UPDATE_ERROR_WRITE:
                    strcat(errorMessage, ERROR_WRITE);
                    break;
                case UPDATE_ERROR_ERASE:
                    strcat(errorMessage, ERROR_ERASE);
                    break;
                case UPDATE_ERROR_READ:
                    strcat(errorMessage, ERROR_READ);
                    break;
                case UPDATE_ERROR_SPACE:
                    strcat(errorMessage, ERROR_SPACE);
                    break;
                case UPDATE_ERROR_SIZE:
                    strcat(errorMessage, ERROR_SIZE);
                    break;
                case UPDATE_ERROR_STREAM:
                    strcat(errorMessage, ERROR_STREAM);
                    break;
                case UPDATE_ERROR_MD5:
                    strcat(errorMessage, ERROR_MD5);
                    break;
                case UPDATE_ERROR_MAGIC_BYTE:
                    strcat(errorMessage, ERROR_MAGIC_BYTE);
                    break;
                case UPDATE_ERROR_NO_PARTITION:
                    strcat(errorMessage, ERROR_NO_PARTITION);
                    break;
                case UPDATE_ERROR_BAD_ARGUMENT:
                    strcat(errorMessage, ERROR_BAD_ARGUMENT);
                    break;
                default:
                    strcat(errorMessage, ERROR_UNKNOWN);
                    break;
            }

            Serial.println(errorMessage);    // Print error message
            otaErrorCallback(errorMessage);  // Send error callback
        }
    } else {
        Serial.println("Not enough space to begin OTA.");
        otaErrorCallback(ERROR_NOT_ENOUGH_SPACE);
    }
}