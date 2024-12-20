#include "webIO.h"

// TODO send out message body of POST commands
const String espSerial::help()
{
    static const String helptext = 
        "Help on Serial subsystem\r\n"
        "\nconfiguration\r\n"
        "  begin=baudrate\r\n"
        "  setBaud=baudrate\r\n"
        "  txPin=pin\r\n"
        "  rxPin=pin\r\n"
        "  setTerm=CR|LF|CRLF\r\n"
        "\nwriting output\r\n"
        "  write=text ... write out as is\r\n"
        "  writeln=text ... write out adding configured line termination\r\n"
        "\nreading input\r\n"
        "  read ... return all characters available in buffer\r\n"
        "  readln ... return single line (removing configured line termination)"
        "NOTE: settings default to 8,N,1\r\n";
    return helptext;
}

void espSerial::setTerm(String value)
{
    if (value == "CR")
        _endOfLine = "\r";
    else if (value == "LF")
        _endOfLine = "\n";
    else if (value == "CRLF")
        _endOfLine = "\r\n";
    else   
        _endOfLine = ""; // TODO report error?
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

String espSerial::parse(String command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    if (command == "begin")
        _serial.begin(value.toInt());
    else if (command == "baud")
        _serial.updateBaudRate(value.toInt());
    else if (command == "rxPin")
        _serial.setPins(value.toInt(), -1);
    else if (command == "txPin")
        _serial.setPins(-1, value.toInt());
    else if (command == "setTerm")
        setTerm(value);
    else if (command == "write")
        write(value);
    else if (command == "writeln")
        writeln(value);
    else if (command == "read")
        result = read();
    else if (command == "readln")
        result = readln();
    else
        result = "invalid keyword " + command;
    return result;
}
