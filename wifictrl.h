#ifndef WIFICTRL_H_
#define WIFICTRL_H_

#ifndef CONFIG_ESP32_PHY_MAX_TX_POWER
    #define CONFIG_ESP32_PHY_MAX_TX_POWER CONFIG_ESP_PHY_MAX_WIFI_TX_POWER
#endif

#include <WiFiManager.h>

WiFiManager wifiManager;

class WiFiCtrl
{
public:
    void check()
    {
        if(!WiFi.isConnected())
        {
            Serial.print("\nAttempting WiFi reconnect");
            WiFi.begin();
            static uint8_t tries = 120;

            while(!WiFi.isConnected())
            {
                if(!tries--)
                {
                    Serial.println("\nfailed! -> Reset");
                    delay(2500);
                    ESP.restart();
                }
                Serial.print('.');
                delay(1000);
            }
            Serial.println(" connected.");
        }       
    }

    void setupWifiPortal(String hostName)
    {
        Serial.print("Attempting WiFi connection... ");
        WiFi.mode(WIFI_STA);
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(hostName.c_str()); //define hostname

        wifiManager.setDebugOutput(false);
        wifiManager.setConfigPortalTimeout(120);

        bool res;
        res = wifiManager.autoConnect(hostName.c_str());

        if (!res)
        {
            Serial.println("failed! -> Reset");
            delay(2500);
            ESP.restart();
        }
        else
        {
            Serial.printf("connected, IP: %s\n", WiFi.localIP().toString().c_str());
        }        
    }
};

static WiFiCtrl wifictrl;


#endif // WIFICTRL_H_