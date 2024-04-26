#ifndef OTAM_STORE_H
#define OTAM_STORE_H

#include <Preferences.h>

class OtamStore {
   public:
    static String readDeviceGuidFromStore();
    static void writeDeviceGuidToStore(String deviceGuid);
    static int readFirmwareUpdateFileIdFromStore();
    static void writeFirmwareUpdateFileIdToStore(int firmwareUpdateFileId);
    static int readFirmwareUpdateIdFromStore();
    static void writeFirmwareUpdateIdToStore(int firmwareUpdateId);
    static String readFirmwareUpdateNameFromStore();
    static void writeFirmwareUpdateNameToStore(String firmwareUpdateName);
    static String readFirmwareUpdateVersionFromStore();
    static void writeFirmwareUpdateStatusToStore(String firmwareUpdateStatus);
    static String readFirmwareUpdateStatusFromStore();
    static void writeFirmwareUpdateVersionToStore(String firmwareUpdateVersion);
};

#endif  // OTAM_STORE_H