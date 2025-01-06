#include "webIO.h"
#include "streamIO.h"

void streamIO::nextArg(String &args)
{
    // extract next argument from list
    String arg = nextString(args, "&");
    // split into name and value
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

String streamIO::streamJSON(espRoot &base, String args)
{
    String message = "";
    if (args.length() == 0)
        return base.help(); // plain text
    while (args.length() > 0)
    {
        nextArg(args);
        String response = base.parse(argName, argValue);
        addResponse(message, response, ",\r\n");
    }
    return "{\r\n" + message + "\r\n}"; // JSON string   
}

String streamIO::streamSerial(espSerial &base, String args)
{
    String message = "";
    if (args.length() == 0)
        return base.help(); // plain text
    else
        return streamJSON(base, args);
}

// non blocking parsing of input stream
// read full line up to newline
// then parse that as it would be url in httpIO, e.g.
// /GPIO?set=17
// result is written back to stream
void streamIO::parse(Stream & s)
{
    char c=0;
    int pos;
    String url;
    String args;

    while (((pos=rxBuffer.indexOf(_endOfBlock))<0) && (s.available()))
    {   // TODO make more efficient
        s.readBytes(&c, 1);
        rxBuffer += c;
    }
    if (pos<0) // still no end of line after reading all input available 
        return;
    String line = rxBuffer.substring(0, pos); // all up to configured end of line 
    rxBuffer = rxBuffer.substring(pos + _endOfBlock.length()); // remove part read
    int argStart = line.indexOf("?");
    if (argStart > 0)
    { // has arguments, split into url and arguments
        url  = line.substring(0,argStart);
        args = line.substring(argStart+1);
    }
    else
    { // no arguments
        url = line;
        args = "";
    }
    String message;
    if (url == "/")
        message = webRoot.help();
    else if (url == "/status")
        message = webRoot.status();
    else if (url == "/GPIO")
        message = streamJSON(webGPIO, args);
    else if (url == "/Serial")
        message = streamSerial(webSerial0, args);
    else if (url == "/Serial0")
        message = streamSerial(webSerial0, args);
    else if (url == "/Serial1")
        message = streamSerial(webSerial1, args);
    else if (url == "/Serial2")
        message = streamSerial(webSerial2, args);
    else if (url == "/DAC1")
        message = streamJSON(webDAC1, args);
    else if (url == "/DAC2")
        message = streamJSON(webDAC2, args);
    else if (url == "/ADC")
        message = streamJSON(webADC, args);
    else if (url == "/PWM")
        message = streamJSON(webPWM, args);
    else if (url == "/I2C")
        message = streamJSON(webI2C0, args);
    else if (url == "/I2C0")
        message = streamJSON(webI2C0, args);
    else if (url == "/I2C1")
        message = streamJSON(webI2C1, args);
    else if (url == "/SPI")
        message = streamJSON(webVSPI, args); // Arduino default SPI is VSPI
/*
    else if (url == "/FSPI") // SPI1 reserved for Flash memory access
        message = streamJSON(webFSPI, args);
*/
    else if (url == "/HSPI")
        message = streamJSON(webHSPI, args);
    else if (url == "/VSPI")
        message = streamJSON(webVSPI, args);
    else
        s.println("undefined url: \"" + url + "\""); 
    s.print(message + _endOfBlock);
    s.flush();
}

streamIO webStream("\004"); // default object
