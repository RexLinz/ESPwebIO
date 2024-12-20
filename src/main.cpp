#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
//#include "networkHome.h" // defines for home network
//#include "networkOffice.h" // defines for office network
#include "webIO.h"

// if none of the includes defining network credentials, set up here
// if CREATE_AP is nonzero, create our own access point
#ifndef _NETWORK_H_
#define CREATE_AP 1
#define WIFI_SSID "MyWIFI"
#define WIFI_PASS "MyWIFIpass"
#endif

// this application's host name to publish on network
#define WIFI_HOST "WebIO"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// connect to WiFi network
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

#define WEBIO_VERSION "WebIO version 1.4.0\r\n"
void handleRoot(AsyncWebServerRequest *request) 
{
    String message = 
        WEBIO_VERSION
        "available subsystems\r\n"
        "  /status ... returning system status\r\n"
        "  /GPIO\r\n"
        "  /Serial = /Serial0, /Serial1, Serial2\r\n"
        "  /DAC1, /DAC2\r\n"
        "  /ADC\r\n"
        "requesting any subsystem without further parameters will return subsystem's help\r\n";
    request->send(200, "text/plain", message); // application/json
}

// display status information
void handleStatus(AsyncWebServerRequest *request)
{
    String message = 
        WEBIO_VERSION
        "Free heap " + String(ESP.getFreeHeap()) + " bytes, largest block " +String(ESP.getMaxAllocHeap()) + " bytes\r\n"
        "Network \"" + WiFi.SSID() + "\" RSSI " + String(WiFi.RSSI()) + " dB\r\n";
    request->send(200, "text/plain", message); // application/json
}

// handle /GPIO requests
void handleGPIO(AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", webGPIO.help());
    else 
    { // process all arguments from request
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

// async serial interfaces
void handleSerial(espSerial &serial, AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", serial.help());
    // process all arguments from request
    else 
    {
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = serial.parse(request->argName(i), request->arg(i));
            if (response.length() > 0)
            {
                if (message.length() > 0)
                    message += "\r\n";
                message += response;
            }
        }
        request->send(200, "text/plain", message);
    }
}

// handle /Serial0 requests (equals /Serial)
void handleSerial0(AsyncWebServerRequest *request)
{
    handleSerial(webSerial0, request);
}

// handle /Serial1 requests
void handleSerial1(AsyncWebServerRequest *request)
{
    handleSerial(webSerial1, request);
}

// handle /Serial2 requests
void handleSerial2(AsyncWebServerRequest *request)
{
    handleSerial(webSerial2, request);
}

// handle body data, forward to Serialx output
// TODO handle actual serial output (not just debugging)
void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if(!index){
        Serial.print("URL: " + request->url());
        Serial.print(" args=" + String(request->args()));
        Serial.printf("BodyStart: %u B\n", total);
    }
    for(size_t i=0; i<len; i++){
        Serial.write(data[i]);
    }
    if(index + len == total){
        Serial.printf("BodyEnd: %u B\n", total);
    }
}

// handle DAC requests
void handleDAC(espDAC &dac, AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", dac.help());
    else 
    { // process all arguments from request
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = dac.parse(request->argName(i), request->arg(i));
            if (response.length() > 0)
            {
                if (message.length() > 0)
                    message += "\r\n";
                message += response;
            }
        }
        request->send(200, "text/plain", message);
    }
}

void handleDAC1(AsyncWebServerRequest *request)
{
    handleDAC(webDAC1, request);
}

void handleDAC2(AsyncWebServerRequest *request)
{
    handleDAC(webDAC2, request);
}

// handle ADC requests
void handleADC(AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", webADC.help());
    else 
    { // process all arguments from request
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = webADC.parse(request->argName(i), request->arg(i));
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

    // application version and general help 
    server.on("/", handleRoot);
    // .../status -> display some status information
    server.on("/status", handleStatus);
    // .../GPIO -> access digital IO pins
    server.on("/GPIO", handleGPIO);
    // async serial interfaces
    server.on("/Serial",  handleSerial0); // Serial equals Serial0
    server.on("/Serial0", handleSerial0);
    server.on("/Serial1", handleSerial1);
    server.on("/Serial2", handleSerial2);
    // digital to analog converters
    server.on("/DAC1", handleDAC1);
    server.on("/DAC2", handleDAC2);
    // analog to digital converter
    server.on("/ADC", handleADC);

    // body data handling for binary IO to serial
    // https://github.com/me-no-dev/ESPAsyncWebServer#body-data-handling
    // https://github.com/me-no-dev/ESPAsyncWebServer/issues/123
    server.onRequestBody(handleBody);  
    
    // ... not found -> display error message with debugging information
    server.onNotFound(handleNotFound);
    server.begin();
}

void loop() 
{
    while (1)
        ; // empty main loop, all is handled in web server
}
