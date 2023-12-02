#include <HTTPClient.h>
#include <Update.h>
#include <OtamDevice.h>

struct OTAMConfig
{
    String url;
    String firmwareId;
    String deviceId = "";
    String deviceName = "";
    String currentFirmwareVersion = "";
};

class OTAMClient
{
private:
    OTAMConfig otamConfig;
    String firmware_url = "";

public:
    OTAMClient(OTAMConfig config)
    {
        otamConfig = config;

        // Set the URL
        otamConfig.url = config.url;

        // Set the device ID, generate one if not provided
        otamConfig.deviceId = readOrGenerateDeviceID(config.deviceId);

        // Output the device ID
        Serial.println("Device ID: " + otamConfig.deviceId);

        // Set the firmware ID
        otamConfig.firmwareId = config.firmwareId;

        // Set the firmware URL
        firmware_url = otamConfig.url + "/firmware/" + otamConfig.firmwareId;

        // Set the device name, use a default value if not provided
        otamConfig.deviceName = config.deviceName.isEmpty() ? "DefaultDeviceName" : config.deviceName;

        // Set the current firmware version
        otamConfig.currentFirmwareVersion = config.currentFirmwareVersion;
    }

    boolean hasPendingUpdate()
    {
        Serial.println("Checking for firmware update");

        HTTPClient http;

        String check_url = firmware_url + "/check?deviceId=" + otamConfig.deviceId + "&deviceName=" + otamConfig.deviceName + "&currentFirmwareVersion=" + otamConfig.currentFirmwareVersion;

        Serial.println("Firmware check url: " + check_url);

        http.begin(check_url);

        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();

            Serial.println("Payload: " + payload);

            // Get the update available flag
            if (payload == "true")
            {
                Serial.println("Firmware update available");
                return true;
            }
            else
            {
                Serial.println("No firmware update available");
                return false;
            }
        }
        else
        {
            Serial.println("HTTP request failed");
        }

        http.end();
        Serial.println("Firmware check finished");
        return false;
    }

    void doFirmwareUpdate()
    {
        Serial.println("Firmware update started");

        HTTPClient http;

        String download_url = firmware_url + "/download";

        Serial.println("Firmware download url: " + download_url);

        http.begin(download_url);

        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_NO_CONTENT) // Status 204: Firmware already up to date
        {
            Serial.println("No new firmware available");
            return;
        }
        else if (httpCode == HTTP_CODE_OK) // Status 200: Download available
        {
            int contentLength = http.getSize();

            bool canBegin = Update.begin(contentLength);
            if (canBegin)
            {
                WiFiClient *client = http.getStreamPtr();

                // Write the downloaded binary to flash memory
                size_t written = Update.writeStream(*client);

                if (written == contentLength)
                {
                    Serial.println("Written : " + String(written) + " successfully");
                }
                else
                {
                    Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                }

                if (Update.end())
                {
                    Serial.println("OTA done!");
                    if (Update.isFinished())
                    {
                        Serial.println("Update successfully completed. Rebooting.");
                        ESP.restart();
                    }
                    else
                    {
                        Serial.println("Update not finished? Something went wrong!");
                    }
                }
                else
                {
                    Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                }
            }
            else
            {
                Serial.println("Not enough space to begin OTA");
            }
        }
        else
        {
            Serial.println("HTTP request failed");
        }

        http.end();
        Serial.println("Firmware update finished");
    }
};