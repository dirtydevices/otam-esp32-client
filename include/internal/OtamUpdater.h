#ifndef OTAM_UPDATER_H
#define OTAM_UPDATER_H

#include <HTTPClient.h>
#include <Update.h>
#include <functional>

class OtamUpdater {
   public:
    // Define the type for the callback functions
    using CallbackType = std::function<void()>;
    using NumberCallbackType = std::function<void(int)>;
    using StringCallbackType = std::function<void(String)>;

    // Variables to hold the callback functions
    CallbackType otaAfterDownloadCallback;
    NumberCallbackType otaDownloadProgressCallback;
    CallbackType otaSuccessCallback;
    StringCallbackType otaErrorCallback;

    // Define the callback functions
    void onOtaAfterDownload(CallbackType afterDownloadCallback);
    void onOtaDownloadProgress(NumberCallbackType progressCallback);
    void onOtaSuccess(CallbackType successCallback);
    void onOtaError(StringCallbackType errorCallback);
    void runESP32Update(HTTPClient& http);
};

#endif  // OTAM_UPDATER_H