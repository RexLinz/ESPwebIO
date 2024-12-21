#include "webIO.h"
#include "streamIO.h"

void streamIO::nextArg(String &args)
{
    // extract next argument from list
    String arg = espUtil::nextString(args, "&");
    // split into name and value
    String argName;
    String argValue;
    int valPos = arg.indexOf("=");
    if (valPos >= 0)
    {
        argName  = arg.substring(0,valPos);
        argValue = arg.substring(valPos+1);
    }
    else
    {
        argName  = arg;
        argValue = "";
    }
}

String streamIO::streamGPIO(String args)
{
//    Serial.println(args); // for debugging of keyboard input
    String message = "";
    if (args.length() == 0)
        return webGPIO.help; // plain text
    while (args.length() > 0)
    {
        nextArg(args);
        String response = webGPIO.parse(argName, argValue);
        addResponse(message, response);
    }
    return "{\r\n" + message + "\r\n}\r\n"; // JSON string
}

String streamIO::streamSerial(espSerial &serial, String args)
{
    String message = "";
    if (args.length() == 0)
        return espSerial::help; // plain text
    while (args.length() > 0)
    {
        nextArg(args);
        String response = serial.parse(argName, argValue);
        addResponse(message, response);
    }
    return message; // plain text
}

String streamIO::streamDAC(espDAC &DAC, String args)
{
    String message = "";
    if (args.length() == 0)
        return espDAC::help; // plain text
    while (args.length() > 0)
    {
        nextArg(args);
        String response = DAC.parse(argName, argValue);
        addResponse(message, response);
    }
    return message; // plain text
}

String streamIO::streamADC(String args)
{
    String message = "";
    if (args.length() == 0)
        return espADC::help; // plain text
    while (args.length() > 0)
    {
        nextArg(args);
        String response = webADC.parse(argName, argValue);
        addResponse(message, response);
    }
    return "{\r\n" + message + "\r\n}\r\n"; // JSON string
}

// non blocking parsing of input stream
// read full line up to newline
// then parse that as it would be url in httpIO, e.g.
// /GPIO?set=17
// result is written back to stream
// TODO make line termination configurable
void streamIO::parse(Stream & s)
{
    char c=0;
    int pos;
    String url;
    String args;

    while (((pos=rxBuffer.indexOf(_endOfLine))<0) && (s.available()))
    {   // TODO make more efficient
        s.readBytes(&c, 1);
        rxBuffer += c;
    }
    if (pos<0) // still no end of line after reading all input available 
        return;
    String line = rxBuffer.substring(0, pos); // all up to configured end of line 
    rxBuffer = rxBuffer.substring(pos + _endOfLine.length()); // remove part read
    int argStart = rxBuffer.indexOf("?");
    if (argStart > 0)
    { // has arguments, split into url and arguments
        url  = rxBuffer.substring(0,argStart);
        args = rxBuffer.substring(argStart+1);
    }
    else
    { // no arguments
        url = rxBuffer;
        args = "";
    }
    if (url == "/")
        s.print(espUtil::help);
    else if (url == "/status")
        s.print(espUtil::status());
    else if (url == "/GPIO")
        s.print(streamGPIO(args));
    else if (url == "/Serial")
        s.print(streamSerial(webSerial0, args));
    else if (url == "/Serial0")
        s.print(streamSerial(webSerial0, args));
    else if (url == "/Serial1")
        s.print(streamSerial(webSerial1, args));
    else if (url == "/Serial2")
        s.print(streamSerial(webSerial2, args));
    else if (url == "/DAC1")
        s.print(streamDAC(webDAC1, args));
    else if (url == "/DAC2")
        s.print(streamDAC(webDAC2, args));
    else if (url == "/ADC")
        s.print(streamADC(args));
    else
        s.println("undefined url: \"" + url + "\""); 
    rxBuffer = "";
}

streamIO webStream("\r\n"); // default object
