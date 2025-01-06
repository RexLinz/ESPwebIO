#include "webIO.h"

// TODO send out message body of POST commands
const String espSerial::help()
{
    return
        "Help on Serial subsystem\r\n"
        "\nconfiguration\r\n"
        "  txPin=pin\r\n"
        "  rxPin=pin\r\n"
        "  txBufferSize=size\r\n"
        "  rxBufferSize=size\r\n"
        "  setTerm=CR|LF|CRLF\r\n"
        "  setBaud=baudrate\r\n"
        "  begin=baudrate\r\n"
        "\nwriting output\r\n"
        "  write=text ... write out as is\r\n"
        "  writeln=text ... write out adding configured line termination\r\n"
        "\nreading input\r\n"
        "  read ... return all characters available in buffer\r\n"
        "  readln ... return single line (removing configured line termination)"
        "NOTE: settings default to 8,N,1";
}

String espSerial::setTerm(String &value)
{
    if (value == "CR")
        _endOfLine = "\r";
    else if (value == "LF")
        _endOfLine = "\n";
    else if (value == "CRLF")
        _endOfLine = "\r\n";
    else
        return "\"invalid termination\"";
    return value;
}

String espSerial::read()
{
    char c;
    while (_serial.available())
    {   // TODO make more efficient
        _serial.read(&c, 1);
        rxBuffer += c;
    }
    String temp = rxBuffer;
    rxBuffer = "";
    return temp;
}

String espSerial::readln()
{
    char c;
    int pos;
    if (_endOfLine.length() == 0)
        return read(); // no end of line configured
    while (((pos=rxBuffer.indexOf(_endOfLine))<0) && (_serial.available()))
    {   // TODO make more efficient
        _serial.read(&c, 1);
        rxBuffer += c;
    }
    if (pos<0) // still no end of line after reading all input available 
        return "";
    String s = rxBuffer.substring(0, pos); // all up to configured end of line 
    rxBuffer = rxBuffer.substring(pos + _endOfLine.length()); // remove part read
    return s;
}

String espSerial::parse(const String &command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    if (command == "begin")
        result = begin(value.toInt());
    else if (command == "baud")
        result = setBaud(value.toInt());
    else if (command == "txPin")
        result = txPin(value.toInt());
    else if (command == "rxPin")
        result = rxPin(value.toInt());
    else if (command == "txBufferSize")
        result = txBufferSize(value.toInt());
    else if (command == "rxBufferSize")
        result = rxBufferSize(value.toInt());
    else if (command == "setTerm")
        result = setTerm(value);
    else if (command == "write")
        result = write(value);
    else if (command == "writeln")
        result = writeln(value);
    else if (command == "read")
        return read();
    else if (command == "readln")
        return readln();
    else
        result = "\"invalid keyword\"";
    return result;
}
