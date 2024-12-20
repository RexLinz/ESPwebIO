#include "webIO.h"
#include "streamIO.h"

String streamIO::GPIO(String args)
{
    Serial.println(args); // for debugging of keyboard input
    String message = "";
    if (args.length() == 0)
        return webGPIO.help; // plain text
    while (args.length() > 0)
    {
        // extract next argument from list
        String arg = espUtil::nextString(args,"&");
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
        String response = webGPIO.parse(argName, argValue);
        if (response.length() > 0)
        {
            if (message.length() > 0)
                message += ",\r\n";
            message += response;
        }
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
        s.print(GPIO(args));
    else
        s.println("undefined url: \"" + url + "\""); 
    rxBuffer = "";
}

streamIO webStream("\r\n"); // default object
