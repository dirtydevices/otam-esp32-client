#include "internal/OtamStore.h"

String OtamStore::readDeviceGuidFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
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
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (deviceGuid.length() == 0) {
        if (preferences.getString("device_guid", "").length() > 0) {
            if (!preferences.remove("device_guid")) {
                throw std::runtime_error("Failed to remove device ID from NVS");
                preferences.end();
                return;
            }
        }
    } else if (!preferences.putString("device_guid", deviceGuid)) {
        throw std::runtime_error("Failed to write device ID to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

int OtamStore::readFirmwareUpdateFileIdFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
    }

    int firmwareUpdateFileId = preferences.getInt("file_id");
    preferences.end();
    return firmwareUpdateFileId;
}

void OtamStore::writeFirmwareUpdateFileIdToStore(int firmwareUpdateFileId) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (preferences.putInt("file_id", firmwareUpdateFileId) == 0) {
        throw std::runtime_error("Failed to write firmware file update ID to NVS");
    }

    preferences.end();
}

int OtamStore::readFirmwareUpdateIdFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
    }

    int firmwareUpdateId = preferences.getInt("firmware_id");
    preferences.end();
    return firmwareUpdateId;
}

void OtamStore::writeFirmwareUpdateIdToStore(int firmwareUpdateId) {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (preferences.putInt("firmware_id", firmwareUpdateId) == 0) {
        throw std::runtime_error("Failed to write firmware update ID to NVS");
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateNameFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
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
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (!preferences.putString("firmware_name", firmwareUpdateName)) {
        throw std::runtime_error("Failed to write firmware update name to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateVersionFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
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
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (!preferences.putString("fw_version", firmwareUpdateVersion)) {
        throw std::runtime_error("Failed to write firmware update version to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}

String OtamStore::readFirmwareUpdateStatusFromStore() {
    Preferences preferences;
    if (!preferences.begin("otam-store", false)) {
        throw std::runtime_error("Failed to initialize NVS");
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
        throw std::runtime_error("Failed to initialize NVS");
    }

    if (!preferences.putString("fw_status", firmwareUpdateStatus)) {
        throw std::runtime_error("Failed to write firmware update status to NVS");
        preferences.end();
        return;
    }

    preferences.end();
}