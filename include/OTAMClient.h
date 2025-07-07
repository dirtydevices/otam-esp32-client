#pragma once

#include <EventEmitter.hpp>

#include "Adapter.h"
#include "Updater.h"

namespace OTAM {

    // Events for an OTA updater
    enum class ClientEvent : uint8_t {
        DownloadProgress,  // download percentage complete
        UpdateComplete,    // true on success, false on failure
        FlashProgress,     // flashing progress
        FlashComplete,     // flash complete
        InitReboot,        // before rebooting the device (this is where the consuming firmware reboots)
        COUNT
    };

    class OTAMClient : public EventEmitter<ClientEvent, static_cast<std::size_t>(ClientEvent::COUNT)> {
       public:
        OTAMClient* GetClient(const OtamConfig& config);

        ~OTAMClient();

        // initialize the client
        int8_t init();

        // Check if a firmware update is available
        int8_t checkFirmwareAvailable(bool& available);

        // download the available firmware update
        int8_t downloadFirmware();

        // perform the firmware update
        int8_t updateFirmware();

       private:
        static OTAMClient* _instance;

        OTAMClient(const OtamConfig& config);

        Adapter* _adapter;
        Updater* _updater;

        static void onDownloadProgress(int);
        static void onFlashProgress(int);
        static void onUpdateComplete(bool);
        static void onFlashComplete(bool);
        static void onInitReboot(void*);
    };
}  // namespace OTAM

template <>
struct EventPayload<OTAM::ClientEvent::DownloadProgress> {
    using type = int;
};

template <>
struct EventPayload<OTAM::ClientEvent::UpdateComplete> {
    using type = bool;
};

template <>
struct EventPayload<OTAM::ClientEvent::FlashProgress> {
    using type = int;
};

template <>
struct EventPayload<OTAM::ClientEvent::FlashComplete> {
    using type = bool;
};

template <>
struct EventPayload<OTAM::ClientEvent::InitReboot> {
    using type = void*;
};