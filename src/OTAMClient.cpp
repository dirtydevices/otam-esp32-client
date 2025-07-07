#include "OTAMClient.h"

OTAM::OTAMClient* OTAM::OTAMClient::_instance = nullptr;

OTAM::OTAMClient* OTAM::OTAMClient::GetClient(const OtamConfig& config) {
    if (!_instance) {
        _instance = new OTAMClient(config);
    }
    return _instance;
}

OTAM::OTAMClient::OTAMClient(const OtamConfig& config) {
    _adapter = new Adapter(config);
    _updater = new Updater();

    // bind all of the event handlers for the adapter
    _adapter->on<AdapterEvent::DownloadProgress>(onDownloadProgress);

    // bind all of the event handlers for the updater
    _updater->on<UpdaterEvent::FlashProgress>(onFlashProgress);
    _updater->on<UpdaterEvent::FlashComplete>(onFlashComplete);

    _updater->on<UpdaterEvent::UpdateComplete>(onUpdateComplete);
}

int8_t OTAM::OTAMClient::init() {
    if (_updater->init() < 0) {
        return -2;  // Error: Failed to initialize updater
    }

    if (_adapter->init() < 0) {
        return -1;  // Error: Failed to initialize adapter
    }

    return 0;
}

int8_t OTAM::OTAMClient::checkFirmwareAvailable(bool& available) {
    if (!_adapter->isReady()) {
        return -1;  // Error: Adapter not ready
    }

    // Check for firmware update availability
    return _adapter->updateAvailable(available);
}

int8_t OTAM::OTAMClient::downloadFirmware() {
    if (!_adapter->isReady()) {
        return -1;  // Error: Adapter not ready
    }

    // Download the available firmware
    return _adapter->downloadAvailableFirmware();
}

int8_t OTAM::OTAMClient::updateFirmware() {
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

void OTAM::OTAMClient::onDownloadProgress(int progress) {
    if (_instance) {
        _instance->emit<ClientEvent::DownloadProgress>(progress);
    }
}

void OTAM::OTAMClient::onFlashProgress(int progress) {
    if (_instance) {
        _instance->emit<ClientEvent::FlashProgress>(progress);
    }
}

void OTAM::OTAMClient::onUpdateComplete(bool success) {
    if (_instance) {
        _instance->emit<ClientEvent::UpdateComplete>(success);
    }
}

void OTAM::OTAMClient::onFlashComplete(bool success) {
    if (_instance) {
        _instance->emit<ClientEvent::FlashComplete>(success);
    }
}