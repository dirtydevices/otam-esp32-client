#include "internal/OtamStore.h"

String OtamStore::readDeviceGuidFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readDeviceGuidFromStore");
        return "";
    }

    String deviceGuid;

    if (preferences.isKey("device_guid")) {
        deviceGuid = preferences.getString("device_guid");  // Try to read the device ID from NVS
    }

    preferences.end();
    return deviceGuid;
}

void OtamStore::writeDeviceGuidToStore(String deviceGuid) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeDeviceGuidToStore");
        return;
    }

    if (deviceGuid.length() == 0) {
        if (preferences.getString("device_guid", "").length() > 0) {
            if (!preferences.remove("device_guid")) {
                Serial.println("Error: Failed to remove device ID from NVS");
                preferences.end();
                return;
            }
        }
    } else if (!preferences.putString("device_guid", deviceGuid)) {
        Serial.println("Error: Failed to write device ID to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

int OtamStore::readFirmwareUpdateFileIdFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readFirmwareUpdateFileIdFromStore");
        return 0;
    }

    int firmwareUpdateFileId = preferences.getInt("file_id");
    preferences.end();
    return firmwareUpdateFileId;
}

void OtamStore::writeFirmwareUpdateFileIdToStore(int firmwareUpdateFileId) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeFirmwareUpdateFileIdToStore");
        return;
    }

    if (preferences.putInt("file_id", firmwareUpdateFileId) == 0) {
        Serial.println("Error: Failed to write firmware file update ID to NVS");
    }

    preferences.end();
}

int OtamStore::readFirmwareUpdateIdFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readFirmwareUpdateIdFromStore");
        return 0;
    }

    int firmwareUpdateId = preferences.getInt("firmware_id");
    preferences.end();
    return firmwareUpdateId;
}

void OtamStore::writeFirmwareUpdateIdToStore(int firmwareUpdateId) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeFirmwareUpdateIdToStore");
        return;
    }

    if (preferences.putInt("firmware_id", firmwareUpdateId) == 0) {
        Serial.println("Error: Failed to write firmware update ID to NVS");
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateNameFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readFirmwareUpdateNameFromStore");
        return "";
    }

    String firmwareUpdateName;

    if (preferences.isKey("firmware_name")) {
        firmwareUpdateName = preferences.getString("firmware_name");  // Try to read the device ID from NVS
    }

    preferences.end();
    return firmwareUpdateName;
}

void OtamStore::writeFirmwareUpdateNameToStore(String firmwareUpdateName) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeFirmwareUpdateNameToStore");
        return;
    }

    if (!preferences.putString("firmware_name", firmwareUpdateName)) {
        Serial.println("Error: Failed to write firmware update name to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateVersionFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readFirmwareUpdateVersionFromStore");
        return "";
    }

    String firmwareUpdateVersion;

    if (preferences.isKey("fw_version")) {
        firmwareUpdateVersion = preferences.getString("fw_version");  // Try to read the device ID from NVS
    }

    preferences.end();
    return firmwareUpdateVersion;
}

void OtamStore::writeFirmwareUpdateVersionToStore(String firmwareUpdateVersion) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeFirmwareUpdateVersionToStore");
        return;
    }

    if (!preferences.putString("fw_version", firmwareUpdateVersion)) {
        Serial.println("Error: Failed to write firmware update version to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateStatusFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in readFirmwareUpdateStatusFromStore");
        return "";
    }

    String firmwareUpdateStatus;

    if (preferences.isKey("fw_status")) {
        firmwareUpdateStatus = preferences.getString("fw_status");  // Try to read the device ID from NVS
    }

    preferences.end();
    return firmwareUpdateStatus;
}

void OtamStore::writeFirmwareUpdateStatusToStore(String firmwareUpdateStatus) {
    Preferences preferences;

    if (!preferences.begin("otam-store", false)) {
        Serial.println("Error: Failed to initialize NVS in writeFirmwareUpdateStatusToStore");
        return;
    }

    if (!preferences.putString("fw_status", firmwareUpdateStatus)) {
        Serial.println("Error: Failed to write firmware update status to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}