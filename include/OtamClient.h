#ifndef OTAM_CLIENT_H
#define OTAM_CLIENT_H

#include <HttpClient.h>
#include "internal/OtamConfig.h"
#include "internal/OtamDevice.h"
#include "internal/OtamHttp.h"

struct FirmwareUpdateValues {
    int firmwareFileId;
    int firmwareId;
    String firmwareName;
    String firmwareVersion;
};

class OtamClient {
   private:
    OtamConfig clientOtamConfig;
    OtamDevice* otamDevice;
    bool deviceInitialized = false;
    bool updateStarted = false;
    FirmwareUpdateValues firmwareUpdateValues;
    void sendOtaUpdateError(String logMessage);

   public:
    explicit OtamClient(const OtamConfig& config);
    using EmptyCallbackType = std::function<void()>;
    using NumberCallbackType = std::function<void(int)>;
    using SuccessCallbackType = std::function<void(FirmwareUpdateValues)>;
    using ErrorCallbackType = std::function<void(FirmwareUpdateValues, String)>;
    NumberCallbackType otaDownloadProgressCallback;
    EmptyCallbackType otaBeforeDownloadCallback;
    EmptyCallbackType otaAfterDownloadCallback;
    EmptyCallbackType otaBeforeRebootCallback;
    SuccessCallbackType otaSuccessCallback;
    ErrorCallbackType otaErrorCallback;
    void onOtaDownloadProgress(NumberCallbackType progressCallback);
    void onOtaBeforeDownload(EmptyCallbackType beforeDownloadCallback);
    void onOtaAfterDownload(EmptyCallbackType afterDownloadCallback);
    void onOtaBeforeReboot(EmptyCallbackType beforeRebootCallback);
    void onOtaSuccess(SuccessCallbackType successCallback);
    void onOtaError(ErrorCallbackType errorCallback);
    bool isInitialized();
    void initialize();
    OtamHttpResponse logDeviceMessage(String message);
    boolean hasPendingUpdate();
    void doFirmwareUpdate();
};

#endif  // OTAM_CLIENT_H
