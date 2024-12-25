#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include "networkHome.h" // defines for home network
//#include "networkOffice.h" // defines for office network
#include "webIO.h"
#include "httpIO.h"
#include "streamIO.h"

// if none of the includes defining network credentials, set up here
// if CREATE_AP is nonzero, create our own access point
#ifndef _NETWORK_H_
#define CREATE_AP 1
#define WIFI_SSID "MyWIFI"
#define WIFI_PASS "MyWIFIpass"
#endif

// this application's host name to publish on network
#define WIFI_HOST "WebIO"

//WiFiServer tcpServer(8080);

// connect to WiFi network or create access point
void connectWiFi()
{
#if CREATE_AP>0
    Serial.print("creating access point " + String(WIFI_SSID));
    WiFi.setHostname(WIFI_HOST);
//    WiFi.mode(WIFI_MODE_AP); // access point only
    bool success;
//    success = WiFi.softAP(WIFI_SSID); // access point without password
    success = WiFi.softAP(WIFI_SSID, WIFI_PASS); // using WiFi password
    if (success)
    {
        WiFi.softAPsetHostname(WIFI_HOST);
        Serial.print(", IP Address ");
        Serial.print(WiFi.softAPIP()); // typically 192.168.4.1
        // publish hostname
        if (MDNS.begin(WIFI_HOST)) {
            Serial.print(", Hostname ");
            Serial.print(WiFi.getHostname());
        }
    }
    else 
        Serial.print(" FAILED");
#else
    Serial.print("connecting to WiFi ");
    WiFi.setHostname(WIFI_HOST);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\rIP Address ");
    Serial.print(WiFi.localIP());    
    // publish hostname
    if (MDNS.begin(WIFI_HOST)) {
        Serial.print(", Hostname ");
        Serial.print(WiFi.getHostname());
    }
#endif
    Serial.println();
}

void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("===================");
    Serial.print(WEBIO_VERSION);

    while (Serial.available())
        Serial.read(); // clear input

    connectWiFi(); // connect to WiFi network or create access point
    startHTTP();   // start parsing of http requests
//    tcpServer.begin();
}

// https://docs.arduino.cc/language-reference/en/functions/wifi/server/ 

void loop() 
{
    delay(100); // anything handled in callbacks
/*
    static WiFiClient tcpClient;
    webStream.parse(Serial); // should be able to parse from TCP and UDP as well
    if (tcpClient.connected())
        webStream.parse(tcpClient);
    else
        tcpClient = tcpServer.available();
*/
}
