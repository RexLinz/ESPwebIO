// code to access webIO via HTTP requests
#include <ESPAsyncWebServer.h>
#include "webIO.h"
#include "httpIO.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void httpRoot(AsyncWebServerRequest *request) 
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
void httpStatus(AsyncWebServerRequest *request)
{
    String message = 
        WEBIO_VERSION
        "Free heap " + String(ESP.getFreeHeap()) + " bytes, largest block " +String(ESP.getMaxAllocHeap()) + " bytes\r\n"
        "Network \"" + WiFi.SSID() + "\" RSSI " + String(WiFi.RSSI()) + " dB\r\n";
    request->send(200, "text/plain", message); // application/json
}

// handle /GPIO requests
void httpGPIO(AsyncWebServerRequest *request)
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
void httpSerial(espSerial &serial, AsyncWebServerRequest *request)
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
void httpSerial0(AsyncWebServerRequest *request)
{
    httpSerial(webSerial0, request);
}

// handle /Serial1 requests
void httpSerial1(AsyncWebServerRequest *request)
{
    httpSerial(webSerial1, request);
}

// handle /Serial2 requests
void httpSerial2(AsyncWebServerRequest *request)
{
    httpSerial(webSerial2, request);
}

// handle body data, forward to Serialx output
// TODO handle actual serial output (not just debugging)
void httpBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
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
void httpDAC(espDAC &dac, AsyncWebServerRequest *request)
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

void httpDAC1(AsyncWebServerRequest *request)
{
    httpDAC(webDAC1, request);
}

void httpDAC2(AsyncWebServerRequest *request)
{
    httpDAC(webDAC2, request);
}

// handle ADC requests
void httpADC(AsyncWebServerRequest *request)
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
void httpNotFound(AsyncWebServerRequest *request) 
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

void startHTTP()
{
    // application version and general help 
    server.on("/", httpRoot);
    // .../status -> display some status information
    server.on("/status", httpStatus);
    // .../GPIO -> access digital IO pins
    server.on("/GPIO", httpGPIO);
    // async serial interfaces
    server.on("/Serial",  httpSerial0); // Serial equals Serial0
    server.on("/Serial0", httpSerial0);
    server.on("/Serial1", httpSerial1);
    server.on("/Serial2", httpSerial2);
    // digital to analog converters
    server.on("/DAC1", httpDAC1);
    server.on("/DAC2", httpDAC2);
    // analog to digital converter
    server.on("/ADC", httpADC);

    // body data handling for binary IO to serial
    // https://github.com/me-no-dev/ESPAsyncWebServer#body-data-handling
    // https://github.com/me-no-dev/ESPAsyncWebServer/issues/123
    server.onRequestBody(httpBody);  
    
    // ... not found -> display error message with debugging information
    server.onNotFound(httpNotFound);
    server.begin();    
}
