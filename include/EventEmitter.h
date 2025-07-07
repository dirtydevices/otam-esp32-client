// EventEmitter.hpp
#pragma once

#include <cstddef>
#include <cstdint>

/* --------------------------------------------------------------------------
 *  Event-specific payload map
 *  --------------------------------
 *  Specialise EventPayload<E> for each enum value that needs a payload.
 *  The default definition makes the payload type void (no argument).
 * -------------------------------------------------------------------------- */
template <auto E>
struct EventPayload {
    using type = void*;
};  // default = no payload

/* --------------------------------------------------------------------------
 *  EventEmitter
 *  --------------------------------
 *  • No dynamic allocation
 *  • Compile-time table size  = NumEvents × MaxListeners
 *  • bool returns on on()/off() for flow control
 *  • Duplicate registration guard
 *  • ISR/RTOS safe once you wrap on()/emit() with your own critical section
 * -------------------------------------------------------------------------- */
template <typename EventEnum, std::size_t NumEvents, int MaxListeners = 4>
class EventEmitter {
    using RawListener = void (*)(void*);

   public:
    // ───────────────────────────────────────────────────────────── listeners
    template <EventEnum E>
    using Payload = typename EventPayload<E>::type;

    template <EventEnum E>
    using Listener = void (*)(Payload<E>);

    /// Register |fn| for event E. Returns false if the table is full.
    template <EventEnum E>
    bool on(Listener<E> fn) {
        const std::size_t idx = static_cast<std::size_t>(E);

        for (int i = 0; i < MaxListeners; ++i) {
            if (listeners_[idx][i] == reinterpret_cast<RawListener>(fn))
                return true;  // already there
            if (!listeners_[idx][i]) {
                listeners_[idx][i] = reinterpret_cast<RawListener>(fn);
                return true;  // added
            }
        }
        return false;  // no slot free
    }

    /// Detach |fn| from event E. Returns false if |fn| not found.
    template <EventEnum E>
    bool off(Listener<E> fn) {
        const std::size_t idx = static_cast<std::size_t>(E);

        for (int i = 0; i < MaxListeners; ++i)
            if (listeners_[idx][i] == reinterpret_cast<RawListener>(fn)) {
                listeners_[idx][i] = nullptr;
                return true;
            }
        return false;
    }

    /// Emit event E with payload |arg|.
    template <EventEnum E>
    void emit(Payload<E> arg) const {
        const std::size_t idx = static_cast<std::size_t>(E);

        for (int i = 0; i < MaxListeners; ++i)
            if (listeners_[idx][i])
                reinterpret_cast<Listener<E>>(listeners_[idx][i])(arg);
    }

   private:
    RawListener listeners_[NumEvents][MaxListeners] = {};
};

/**
 * 
 *

 // Events for an OTA updater
enum class OTAEvent : uint8_t
{
    Progress,   // percentage complete
    Complete,   // true on success, false on failure
    COUNT
};

// Payload map
template<> struct EventPayload<OTAEvent::Progress> { using type = int;  };
template<> struct EventPayload<OTAEvent::Complete> { using type = bool; };

// Concrete emitter type
using OTAEmitter =
    GenericEventEmitter<OTAEvent,
                        static_cast<std::size_t>(OTAEvent::COUNT)>;

// Example class
class OTAUpdater : public OTAEmitter
{
public:
    void performUpdate()
    {
        // … update logic …
        emit<OTAEvent::Progress>(42);   // 42 % complete
        // … more logic …
        emit<OTAEvent::Complete>(true); // finished successfully
    }
};

#include "GenericEventEmitter.hpp"

// ─── 1. Define your event handlers ──────────────────────────────────────────
void onProgress(int percent) {
    printf("[OTA] %d %% complete\n", percent);
}

void onComplete(bool ok) {
    printf("[OTA] Update %s!\\n", ok ? "succeeded" : "FAILED");
}

// ─── 2. Set up the updater and attach handlers ─────────────────────────────
OTAUpdater updater;               // derives from OTAEmitter

void setupUpdate()
{
    // Register listeners; bool return lets you assert/log on failure.
    bool ok1 = updater.on<OTAEvent::Progress>(onProgress);
    bool ok2 = updater.on<OTAEvent::Complete>(onComplete);
    assert(ok1 && ok2);           // or handle the error some other way
}

// ─── 3. Run the update somewhere in your code ──────────────────────────────
void loop()
{
    // Pretend we kick off an update:
    updater.performUpdate();

    // …later, maybe we’re done listening to Progress:
    updater.off<OTAEvent::Progress>(onProgress);
    // Now Progress emits silently, but Complete still fires.
}

 * 
 */