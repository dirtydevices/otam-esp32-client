# Usage

```ini
; platformio.ini

[env:adafruit_feather_esp32s3]
platform = espressif32
board = adafruit_feather_esp32s3
framework = arduino
build_flags = -DARDUINO_USB_MODE=1 -std=gnu++17
build_unflags = -std=gnu++11
board_build.f_cpu = 240000000L
monitor_speed = 115200
; lib_deps = ../otam-client
lib_deps = https://bitbucket.org/shetzel84/otam-esp32-client.git
```

```cpp
// main.cpp

#include <HTTPClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <OtamClient.h>

const char *ssid = "";   // Replace with your network SSID (name)
const char *password = ""; // Replace with your network password

void setup()
{
  Serial.begin(115200);
  delay(2000);

  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void send_log_to_server(const String &logMessage)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin("http://172.20.2.161:3000/api/logs"); // currently using locally hosted otam server

    // Specify content type
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST(logMessage);

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Received payload:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Error in HTTP request");
    }

    http.end();
  }
}

void loop()
{
  send_log_to_server("Platform IO Demo is online!");
  do_firmware_update("http://172.20.2.161:3000/api/firmware"); // currently using locally hosted otam server

  delay(5000); // Delay between requests
}
```
