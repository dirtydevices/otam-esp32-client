#pragma once

#include <Update.h>

#include "EventEmitter.h"

// #include <functional>

namespace OTAM {

    // Events for an OTA updater
    enum class UpdaterEvent : uint8_t {
        UpdateComplete,  // true on success, false on failure
        FlashProgress,   // percentage complete
        FlashComplete,   // flash complete
        COUNT
    };

    class Updater : public EventEmitter<UpdaterEvent, static_cast<std::size_t>(UpdaterEvent::COUNT)> {
       public:
        Updater();
        ~Updater();

        int8_t init();

        int8_t validateAndPerformUpdate(uint8_t* data, size_t size);
        const char* getErrorString();
    };

}  // namespace OTAM

template <>
struct EventPayload<OTAM::UpdaterEvent::FlashProgress> {
    using type = int;
};

template <>
struct EventPayload<OTAM::UpdaterEvent::UpdateComplete> {
    using type = bool;
};

template <>
struct EventPayload<OTAM::UpdaterEvent::FlashComplete> {
    using type = bool;
};