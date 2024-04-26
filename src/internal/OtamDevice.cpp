#include "internal/OtamDevice.h"

void OtamDevice::writeIdToStore(String id) {
    OtamStore::writeDeviceGuidToStore(id);
    // Serial.println("Device id written to store: " + id);
}

void OtamDevice::initialize(OtamConfig config) {
    // writeIdToStore("");
    Serial.println("Initializing device with OTAM server");

    // Read the device id from the store
    String deviceGuidStore = OtamStore::readDeviceGuidFromStore();

    if (deviceGuidStore != "") {
        Serial.println("Device GUID read from store: " + deviceGuidStore);
    }

    // Set the init url
    String initUrl = config.url + "/init-device";

    // Set the payload
    // With deviceId, deviceGuid, deviceProfileId
    String payload = "{\"deviceId\":\"" + config.deviceId + "\",\"deviceGuid\":\"" + deviceGuidStore +
                     "\",\"deviceProfileId\":" + config.deviceProfileId + "}";

    Serial.println("Calling http post with payload: " + payload);

    // Call the init endpoint
    OtamHttpResponse response = OtamHttp::post(initUrl, payload);

    Serial.println("Received response from server");

    if (response.httpCode == 200) {
        // Device found in OTAM DB
        Serial.println("Device GUID returned from OTAM server: " + response.payload);
        // Set the device guid
        deviceGuid = response.payload;
        // Write the device guid to the store
        writeIdToStore(response.payload);
        // Log success
        Serial.println("Device has been initialized with OTAM server");
    } else {
        Serial.println("Error Status code: " + response.httpCode);
        Serial.println("Error Payload: " + response.payload);
    }
}

OtamDevice::OtamDevice(OtamConfig config) {
    // Initialize device with OTAM server
    initialize(config);

    // Set the device URL
    deviceUrl = config.url + "/devices/" + deviceGuid;

    // Set the device log URL
    deviceLogUrl = this->deviceUrl + "/log";

    // Set the device status URL
    deviceStatusUrl = this->deviceUrl + "/status";

    // Set the device download URL
    deviceFirmwareFileUrl = this->deviceUrl + "/firmware-file-url";
}