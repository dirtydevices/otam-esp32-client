#include "internal/OtamUpdater.h"

void OtamUpdater::onOtaAfterDownload(CallbackType afterDownloadCallback) {
    otaAfterDownloadCallback = afterDownloadCallback;
}

void OtamUpdater::onOtaDownloadProgress(NumberCallbackType progressCallback) {
    otaDownloadProgressCallback = progressCallback;
}

void OtamUpdater::onOtaSuccess(CallbackType successCallback) {
    otaSuccessCallback = successCallback;
}

void OtamUpdater::onOtaError(StringCallbackType errorCallback) {
    otaErrorCallback = errorCallback;
}

void OtamUpdater::runESP32Update(HTTPClient& http) {
    try {
        int contentLength = http.getSize();

        bool canBegin = Update.begin(contentLength);
        if (canBegin) {
            WiFiClient* client = http.getStreamPtr();

            Update.onProgress([this](size_t progress, size_t total) {
                otaDownloadProgressCallback(progress / (total / 100));
            });

            // Write the downloaded binary to flash memory
            size_t written = Update.writeStream(*client);

            // if (written == contentLength) {
            //     Serial.println("Written : " + String(written) + " successfully");
            // } else {
            //     Serial.println("Written only : " + String(written) + "/" + String(contentLength) +
            //                    ". Retry?");
            // }

            if (Update.end()) {
                // Download complete
                otaAfterDownloadCallback();
                if (Update.isFinished()) {
                    // Serial.println("Update successfully completed. Rebooting.");
                    otaSuccessCallback();
                } else {
                    otaErrorCallback("Update did not finish, something went wrong!");
                }
            } else {
                otaErrorCallback("Error Occurred. Error #: " + String(Update.getError()));
            }
        } else {
            otaErrorCallback("Not enough space to begin OTA");
        }

    } catch (const std::runtime_error& e) {
        otaErrorCallback(e.what());
    } catch (const std::exception& e) {
        otaErrorCallback(e.what());
    } catch (...) {
        otaErrorCallback("Unknown error occurred");
    }
}