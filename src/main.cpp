#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "networkHome.h" // defines for home network
//#include "networkOffice.h" // defines for office network
#include "webIO.h"

// if none of the includes defining network credentials, set up here
#ifndef _NETWORK_H_
#define WIFI_SSID "WiFiSSID"
#define WIFI_PASS "WiFiPASS"
#endif

// this application's host name to publish on network
#define WIFI_HOST "WebIO"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// connect to WiFi network
void connectWiFi()
{
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
    Serial.println();
}

void handleRoot(AsyncWebServerRequest *request) 
{
    // TODO return main help ans status?
}

// display status information
void handleStatus(AsyncWebServerRequest *request)
{
    String message = "WebIO version 1.0 application status\n";
    message += "Free heap " + String(ESP.getFreeHeap()) + " bytes, largest block " +String(ESP.getMaxAllocHeap()) + " bytes\n";
    message += "Network \"" + WiFi.SSID() + "\" RSSI " + String(WiFi.RSSI()) + " dB\n";
    request->send(200, "text/plain", message); // application/json
}

// handle /GPIO requests
void handleGPIO(AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", webGPIO.help());
    // process all arguments from request
    // TODO if request->args()==0 return HTML help text
    else 
    {
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = webGPIO.parse(request->argName(i), request->arg(i));
            if (response.length() > 0)
            {
                if (message.length() > 0)
                    message += ",\r\n";
                message += response;
            }
        }
        request->send(200, "application/json", "{\r\n" + message + "\r\n}");
    }
}

// display some debugging information about any
// request not handled else
void handleNotFound(AsyncWebServerRequest *request) 
{
    String message = "url not found\n\n";
    message += "URL: "; message += request->url();
    message += "\nMethod: "; message += request->methodToString();
    message += "\nArguments: "; message += request->args(); 
    message += "\n";
    for (uint8_t i = 0; i < request->args(); i++) 
    {
        message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }
    request->send(200, "text/plain", message);
}

void setup() 
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("==================");
    Serial.println("ESP WebIO starting");
    Serial.println();

    while (Serial.available())
        Serial.read(); // clear input

    connectWiFi();

    // .../status -> display some status information
    server.on("/status", handleStatus);
    // .../GPIO -> access digital IO pins
    server.on("/GPIO", handleGPIO);

    // define some request handlers
    // .../ -> root access displays file system content
    // server.on("/", handleRoot);
    
    // ... not found -> display error message with debugging information
    server.onNotFound(handleNotFound);
    server.begin();
}

void loop() 
{
    while (1)
        ; // empty main loop, all is handled in web server
}
