// code to access webIO via HTTP requests
#include <ESPAsyncWebServer.h>
#include "webIO.h"
#include "httpIO.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// root help
void httpRoot(AsyncWebServerRequest *request) 
{
    request->send(200, "text/plain", webRoot.help());
}

// display status information
void httpStatus(AsyncWebServerRequest *request)
{
    String message = webRoot.status() + 
        "Network \"" + WiFi.SSID() + "\" RSSI " + String(WiFi.RSSI()) + " dB\r\n";
    request->send(200, "text/plain", message);
}

// parse the arguments using the parser from the given object
// return JSON formatted string (except for help)
void httpJSON(espRoot &base, AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", base.help());
    else 
    { // process all arguments from request
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = base.parse(request->argName(i), request->arg(i));
            base.addResponse(message, response, ",\r\n");
        }
        request->send(200, "application/json", "{\r\n" + message + "\r\n}");
    }
}

/*
// parse the arguments using the parser from the given object
// return plain text string
void httpText(espRoot &base, AsyncWebServerRequest *request)
{
    String message = "";
    if (request->args() == 0)
        request->send(200, "text/plain", base.help());
    // process all arguments from request
    else 
    {
        for (uint8_t i = 0; i < request->args(); i++) 
        {
            String response = base.parse(request->argName(i), request->arg(i));
            base.addResponse(message, response, "\r\n");
        }
        request->send(200, "text/plain", message);
    }
}
*/

// /GPIO callback
void httpGPIO(AsyncWebServerRequest *request)
{
    httpJSON(webGPIO, request);
}

void httpSerial(espRoot &base, AsyncWebServerRequest *request)
{
    bool asText = false;
    if ((request->args()==1) &&  (request->argName(0U).substring(0,4)=="read")) 
    {   // we have just read or readln?
        // -> these respond sending plain text!
        String response = base.parse(request->argName(0U), request->arg(0U));
        request->send(200, "text/plain", response);
    }
    else
        httpJSON(base, request);
}

// /Serial0 callback (equals /Serial)
void httpSerial0(AsyncWebServerRequest *request)
{
//    httpText(webSerial0, request);
    httpSerial(webSerial0, request);
}

// /Serial1 callback
void httpSerial1(AsyncWebServerRequest *request)
{
//    httpText(webSerial1, request);
    httpSerial(webSerial1, request);
}

// /Serial2 callback
void httpSerial2(AsyncWebServerRequest *request)
{
//    httpText(webSerial2, request);
    httpSerial(webSerial2, request);
}

// handle body data, forward to Serialx output
// function is untested, as I have no client program which will send body content
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

// DAC1 callback
void httpDAC1(AsyncWebServerRequest *request)
{
    httpJSON(webDAC1, request);
}

// DAC2 callback
void httpDAC2(AsyncWebServerRequest *request)
{
    httpJSON(webDAC2, request);
}

// ADC callbak
void httpADC(AsyncWebServerRequest *request)
{
    httpJSON(webADC, request);
}

// display some debugging information 
// for any request not handled else
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

// set up callback functions and start the http server
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
