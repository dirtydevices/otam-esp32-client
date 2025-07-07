#include "Client.h"

OTAM::Client* OTAM::Client::_instance = nullptr;

OTAM::Client* OTAM::Client::GetClient(const OtamConfig& config) {
    if (!_instance) {
        _instance = new Client(config);
    }
    return _instance;
}

OTAM::Client::Client(const OtamConfig& config) {
    _adapter = new Adapter(config);
    _updater = new Updater();

    // bind all of the event handlers for the adapter
    _adapter->on<AdapterEvent::DownloadProgress>(onDownloadProgress);

    // bind all of the event handlers for the updater
    _updater->on<UpdaterEvent::FlashProgress>(onFlashProgress);
    _updater->on<UpdaterEvent::FlashComplete>(onFlashComplete);

    _updater->on<UpdaterEvent::UpdateComplete>(onUpdateComplete);
}

int8_t OTAM::Client::init() {
    if (_updater->init() < 0) {
        return -2;  // Error: Failed to initialize updater
    }

    if (_adapter->init() < 0) {
        return -1;  // Error: Failed to initialize adapter
    }

    return 0;
}

int8_t OTAM::Client::checkFirmwareAvailable(bool& available) {
    if (!_adapter->isReady()) {
        return -1;  // Error: Adapter not ready
    }

    // Check for firmware update availability
    return _adapter->updateAvailable(available);
}

int8_t OTAM::Client::downloadFirmware() {
    if (!_adapter->isReady()) {
        return -1;  // Error: Adapter not ready
    }

    // Download the available firmware
    return _adapter->downloadAvailableFirmware();
}

int8_t OTAM::Client::updateFirmware() {
    if (!_adapter->isReady()) {
        return -1;  // Error: Adapter not ready
    }

    if (_adapter->getAvailableFirmwareCompressedSize() == 0 || !_adapter->getAvailableFirmwareCompressedData()) {
        return -2;  // Error: No firmware downloaded
    }

    if (_updater->validateAndPerformUpdate(_adapter->getAvailableFirmwareCompressedData(),
                                           _adapter->getAvailableFirmwareCompressedSize()) < 0) {
        _adapter->logError(_updater->getErrorString());

        return -3;  // Error: Failed to perform update
    }

    emit<ClientEvent::InitReboot>(nullptr);

    // Perform the firmware update
    return 0;
}

void OTAM::Client::onDownloadProgress(int progress) {
    if (_instance) {
        _instance->emit<ClientEvent::DownloadProgress>(progress);
    }
}

void OTAM::Client::onFlashProgress(int progress) {
    if (_instance) {
        _instance->emit<ClientEvent::FlashProgress>(progress);
    }
}

void OTAM::Client::onUpdateComplete(bool success) {
    if (_instance) {
        _instance->emit<ClientEvent::UpdateComplete>(success);
    }
}

void OTAM::Client::onFlashComplete(bool success) {
    if (_instance) {
        _instance->emit<ClientEvent::FlashComplete>(success);
    }
}

// void OTAM::Client::sendOtaUpdateError(String logMessage) {
//     Serial.println("Sending OTA update error: " + logMessage);
//     OtamHttp::post(otamDevice->deviceStatusUrl,
//                    "{\"deviceStatus\":\"UPDATE_FAILED\",\"firmwareFileId\":" + String(firmwareUpdateValues.firmwareFileId) +
//                        ",\"firmwareId\":" + String(firmwareUpdateValues.firmwareId) + ",\"firmwareVersion\":\"" +
//                        firmwareUpdateValues.firmwareVersion + "\",\"logMessage\":\"" + logMessage + "\"}");
// }

// Initialize the OTAM client
// void OTAM::Client::initialize() {
//     if (!deviceInitialized) {
//         // Serial.println("Initializing OTAM client");

//         // Create the device
//         otamDevice = new Adapter(clientOtamConfig);
//         deviceInitialized = true;

//         // If firmware update status success, publish to success callback
//         String firmwareUpdateStatus = OtamStore::readFirmwareUpdateStatusFromStore();
//         // Serial.println("Client Contrcutor: Store -> Firmware update status: " + firmwareUpdateStatus);
//         if (firmwareUpdateStatus == "UPDATE_SUCCESS") {
//             // Serial.println(
//             //     "Firmware update status is UPDATE_SUCCESS, calling OTA success "
//             //     "callback");
//             FirmwareUpdateValues firmwareUpdateSuccessValues;
//             firmwareUpdateSuccessValues.firmwareFileId = OtamStore::readFirmwareUpdateFileIdFromStore();
//             firmwareUpdateSuccessValues.firmwareId = OtamStore::readFirmwareUpdateIdFromStore();
//             firmwareUpdateSuccessValues.firmwareName = OtamStore::readFirmwareUpdateNameFromStore();
//             firmwareUpdateSuccessValues.firmwareVersion = OtamStore::readFirmwareUpdateVersionFromStore();

//             // Clear the firmware update status
//             OtamStore::writeFirmwareUpdateStatusToStore("NONE");

//             // Check if the callback has been set
//             if (otaSuccessCallback) {
//                 // Call the callback with parameters
//                 otaSuccessCallback(firmwareUpdateSuccessValues);
//             }
//         }
//     }
// }

// Log a message to the device log api
// OtamHttpResponse OTAM::Client::logDeviceMessage(String message) {
//     // Send the log entry
//     OtamHttpResponse response = OtamHttp::post(otamDevice->deviceLogUrl, "{\"message\":\"" + message + "\"}");

//     // Return the response
//     return response;
// }

// Perform the firmware update
// void OTAM::Client::doFirmwareUpdate() {
//     if (!otamDevice) {
//         initialize();
//     }

//     updateStarted = true;

//     // Serial.println("Firmware update started");

//     HTTPClient http;

//     // Serial.println("Getting device firmware file url from: " + otamDevice->deviceFirmwareFileUrl);

//     // Get the device status from the server
//     OtamHttpResponse response = OtamHttp::get(otamDevice->deviceFirmwareFileUrl);

//     if (response.httpCode != 200 || response.payload == "") {
//         String error = "Firmware file url request failed, error: " + String(response.httpCode);
//         updateStarted = false;
//         if (otaErrorCallback) {
//             otaErrorCallback(firmwareUpdateValues, error);
//         }
//         sendOtaUpdateError(error);
//         return;
//     }

//     // Serial.println("Downloading firmware file bin from: " + response.payload);

//     // Check if payload begins with http
//     String url = "";
//     if (response.payload.startsWith("http")) {
//         url = response.payload;
//     } else {
//         url = clientOtamConfig.url + response.payload;
//     }

//     http.begin(url);
//     http.addHeader("x-api-key", clientOtamConfig.apiKey);

//     // Publish to the before download callback
//     if (otaBeforeDownloadCallback) {
//         otaBeforeDownloadCallback();
//     }

//     // Start the download
//     int httpCode = http.GET();

//     // Serial.println("HTTP GET response code: " + String(httpCode));

//     if (httpCode == HTTP_CODE_OK) {
//         // Status 200 : Download available

//         // Serial.println("New firmware available");

//         OtamUpdater* otamUpdater = new OtamUpdater();

//         // Subscribe to the OTA download progress callback
//         otamUpdater->onOtaDownloadProgress([this](int progress) {
//             if (otaDownloadProgressCallback) {
//                 otaDownloadProgressCallback(progress);
//             }
//         });

//         // Subscribe to the OTA after download callback
//         otamUpdater->onOtaAfterDownload([this]() {
//             if (otaAfterDownloadCallback) {
//                 otaAfterDownloadCallback();
//             }
//         });

//         otamUpdater->onOtaSuccess([this]() {
//             Serial.println("OTAM: OTA success callback called");

//             Serial.println("OTAM: Updating device status on server with the following values:");
//             Serial.println("POST Url: " + otamDevice->deviceStatusUrl);
//             Serial.println("Firmware file ID: " + String(firmwareUpdateValues.firmwareFileId));
//             Serial.println("Firmware ID: " + String(firmwareUpdateValues.firmwareId));
//             Serial.println("Firmware name: " + firmwareUpdateValues.firmwareName);
//             Serial.println("Firmware version: " + firmwareUpdateValues.firmwareVersion);

//             // Create JSON objects for each field
//             String statusObj = LightJson::createObject("deviceStatus", "UPDATE_SUCCESS");
//             String fileIdObj = LightJson::createObject("firmwareFileId", firmwareUpdateValues.firmwareFileId);
//             String firmwareIdObj = LightJson::createObject("firmwareId", firmwareUpdateValues.firmwareId);
//             String versionObj = LightJson::createObject("firmwareVersion", firmwareUpdateValues.firmwareVersion.c_str());

//             // Merge all objects
//             String payload = LightJson::mergeObjects(statusObj.c_str(), fileIdObj.c_str());
//             payload = LightJson::mergeObjects(payload.c_str(), firmwareIdObj.c_str());
//             payload = LightJson::mergeObjects(payload.c_str(), versionObj.c_str());

//             // Update device on the server
//             OtamHttpResponse response = OtamHttp::post(otamDevice->deviceStatusUrl, payload);

//             Serial.println("OTAM: Post Response - " + response.payload);

//             // Store the updated firmware file id
//             OtamStore::writeFirmwareUpdateFileIdToStore(firmwareUpdateValues.firmwareFileId);
//             // Serial.println("Firmware update file ID stored: " +
//             //                       String(firmwareUpdateValues.firmwareFileId));

//             // Store the updated firmware id
//             OtamStore::writeFirmwareUpdateIdToStore(firmwareUpdateValues.firmwareId);
//             // Serial.println("Firmware update ID stored: " + String(firmwareUpdateValues.firmwareId));

//             // Store the updated firmware name
//             OtamStore::writeFirmwareUpdateNameToStore(firmwareUpdateValues.firmwareName);
//             // Serial.println("Firmware update name stored: " + firmwareUpdateValues.firmwareName);

//             // Store the updated firmware version
//             OtamStore::writeFirmwareUpdateVersionToStore(firmwareUpdateValues.firmwareVersion);
//             // Serial.println("Firmware update version stored: " + firmwareUpdateValues.firmwareVersion);

//             // Store firmware update status
//             OtamStore::writeFirmwareUpdateStatusToStore("UPDATE_SUCCESS");
//             // Serial.println("Firmware update status stored: UPDATE_SUCCESS");

//             // Publish to the on before reboot callback
//             if (otaBeforeRebootCallback) {
//                 otaBeforeRebootCallback();
//             }

//             Serial.println("OTAM: Rebooting device");

//             // Restart the device
//             // ESP.restart();
//             // esp_deep_sleep_start();
//         });

//         otamUpdater->onOtaError([this](String error) {
//             Serial.println("OTA error callback called");
//             updateStarted = false;
//             if (otaErrorCallback) {
//                 otaErrorCallback(firmwareUpdateValues, error);
//             }
//             sendOtaUpdateError(error);
//         });
//         otamUpdater->runESP32Update(http);
//     } else {
//         String error = "Firmware download failed, error: " + String(httpCode);
//         updateStarted = false;
//         if (otaErrorCallback) {
//             otaErrorCallback(firmwareUpdateValues, error);
//         }
//         sendOtaUpdateError(error);
//     }
// }