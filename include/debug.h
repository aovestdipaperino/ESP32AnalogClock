#ifndef DEBUG_H
#define DEBUG_H


#define tcpip_debug

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define ssid_ap_name "wifiname"
#define ssid_pwd "wifipassword"

void handleOTAtask(void *unused)
{
    while (1)
    {
        ArduinoOTA.handle();
        delay(20);
    }
}

class OTA
{
    static bool updating;

public:
    static void begin()
    {
#ifdef tcpip_debug
        if (ssid_ap_name && ssid_ap_name)
        {

            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid_ap_name, ssid_pwd, 0, WiFi.BSSID(0));
            int result;
            while ((result = WiFi.waitForConnectResult()) != WL_CONNECTED)
            {
                Serial.print(result);
                Serial.println(".");
                delay(1000);
            }
            ArduinoOTA.setHostname("myesp32");
            ArduinoOTA
                .onStart([]()
                         {
                    String type;
                    if (ArduinoOTA.getCommand() == U_FLASH)
                        type = "sketch";
                    else // U_SPIFFS
                        type = "filesystem";
                    updating = true;
                    // esp_bt_controller_disable(); // disable bluetooth.
                    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                    Serial.println("Start updating " + type); })
                .onEnd([]()
                       { Serial.println("\nEnd"); })
                .onProgress([](unsigned int progress, unsigned int total)
                            { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
                .onError([](ota_error_t error)
                         {
                    Serial.printf("Error[%u]: ", error);
                    if (error == OTA_AUTH_ERROR)
                        Serial.println("Auth Failed");
                    else if (error == OTA_BEGIN_ERROR)
                        Serial.println("Begin Failed");
                    else if (error == OTA_CONNECT_ERROR)
                        Serial.println("Connect Failed");
                    else if (error == OTA_RECEIVE_ERROR)
                        Serial.println("Receive Failed");
                    else if (error == OTA_END_ERROR)
                        Serial.println("End Failed"); });

            ArduinoOTA.begin();

            Serial.println("Ready");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());

            xTaskCreate(
                handleOTAtask,  /* Task function. */
                "OTA_DEV_TASK", /* String with name of task. */
                10000,          /* Stack size in bytes. */
                NULL,           /* Parameter passed as input of the task */
                25,             /* Priority of the task. */
                NULL);
        }
        else
        {
            Serial.println("Wifi AP not set");
        }
#endif
    }

    static bool isUpdating()
    {
        return updating;
    }
};

bool OTA::updating;
#endif