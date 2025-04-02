#ifndef OTAM_DEVICE_H
#define OTAM_DEVICE_H

#include "internal/LightJson.h"
#include "internal/OtamConfig.h"
#include "internal/OtamHttp.h"
#include "internal/OtamStore.h"
#include "internal/OtamUpdater.h"

class OtamDevice {
   private:
    void writeIdToStore(String id);
    void initialize(OtamConfig config);

   public:
    String deviceGuid;
    String deviceUrl;
    String deviceLogUrl;
    String deviceStatusUrl;
    String deviceInitializeUrl;
    String deviceFirmwareFileUrl;
    explicit OtamDevice(OtamConfig config);
};

#endif  // OTAM_DEVICE_H