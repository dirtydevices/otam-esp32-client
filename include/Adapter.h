#pragma once

#include <Arduino.h>

#include <EventEmitter.hpp>

namespace OTAM {

    struct OtamConfig {
        String apiKey = "";    // user's api key
        String url = "";       // base otam api url
        String deviceId = "";  // device id
        int deviceProfileId;   // device profile id
    };

    struct Firmware {
        int fileId;
        int id;
        String name;
        String version;
        int size;
    };

    enum class AdapterEvent : uint8_t {
        DownloadProgress,  // download percentage complete
        COUNT
    };

    class Adapter : public EventEmitter<AdapterEvent, static_cast<std::size_t>(AdapterEvent::COUNT)> {
       public:
        Adapter(const OtamConfig& config);
        ~Adapter();

        int8_t init();
        int8_t logError(String error);
        int8_t logMessage(String message);
        int8_t updateAvailable(bool& available);
        int8_t downloadAvailableFirmware();

        bool isReady() const { return _ready; }
        String getGUID() const { return _guid; }
        String getDeviceId() const { return _deviceId; }
        int getDeviceProfileId() const { return _deviceProfileId; }

        size_t getAvailableFirmwareCompressedSize() const { return _availableFirmwareCompressedSize; }
        uint8_t* getAvailableFirmwareCompressedData() const { return _availableFirmwareCompressedData; }

        int8_t clearAvailableFirmware();

       private:
        const OtamConfig& _config;

        bool _ready;
        String _guid;
        String _deviceId;
        int _deviceProfileId;

        Firmware _availableFirmware;
        bool _firmwareAvailable;

        String _availableFirmwareURL;

        size_t _availableFirmwareCompressedSize;
        uint8_t* _availableFirmwareCompressedData;

        int8_t initializeDevice();
        int8_t fetchDownloadURL();

        void json_escape(const char* input, char* output, size_t output_size);

        int8_t httpPost(const String& url, const String& payload, String* response = nullptr);
        int8_t httpGet(const String& url, String& response);

        bool extractInt(const String& src, const char* key, int& out);
        bool extractString(const String& src, const char* key, String& out);
    };

}  // namespace OTAM

template <>
struct EventPayload<OTAM::AdapterEvent::DownloadProgress> {
    using type = int;
};