#include "Updater.h"

#include <Preferences.h>

OTAM::Updater::Updater() {}

OTAM::Updater::~Updater() {}

int8_t OTAM::Updater::init() {
    Preferences preferences;

    if (!preferences.begin("otam-store", false)) {
        return -1;  // Error: Failed to initialize NVS in readDeviceGuidFromStore
    }

    bool firmwareUpdateComplete = false;

    if (preferences.isKey("fw_update_complete")) {
        firmwareUpdateComplete = preferences.getBool("fw_update_complete");
    }

    if (firmwareUpdateComplete) {
        if (!preferences.putBool("fw_update_complete", false)) {
            return -2;  // Error: Failed to write firmware update status to store
        }

        emit<UpdaterEvent::UpdateComplete>(true);
    }

    preferences.end();

    return 0;
}

int8_t OTAM::Updater::validateAndPerformUpdate(uint8_t* data, size_t size) {
    // Basic sanity checks
    if (size < 64 * 1024 || size > 2 * 1024 * 1024) {
        return -2;  // Invalid size
    }

    if (!Update.begin(size)) {
        return -3;  // Not enough space
    }

    size_t offset = 0;
    const size_t CHUNK = 1024;
    uint8_t lastPercent = 0;

    while (offset < size) {
        size_t toWrite = min(CHUNK, size - offset);
        size_t written = Update.write(data + offset, toWrite);

        if (written != toWrite) {
            Update.abort();

            emit<UpdaterEvent::FlashComplete>(false);

            return -4;  // Write error
        }

        offset += written;

        // Calculate and update percent complete
        uint8_t percent = static_cast<uint8_t>((100UL * offset) / size);
        if (percent > lastPercent) {
            lastPercent = percent;

            emit<UpdaterEvent::FlashProgress>(percent);
        }
    }

    emit<UpdaterEvent::FlashComplete>(true);

    if (!Update.end()) {
        return -5;  // Error during finalization
    }

    if (!Update.isFinished()) {
        return -6;  // Update not finished
    }

    return 0;  // Success
}

const char* OTAM::Updater::getErrorString() {
    return Update.errorString();
}