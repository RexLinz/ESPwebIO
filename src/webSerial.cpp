#include "webIO.h"

// TODO accept POST commands and send out message body
String espSerial::help()
{
    static const String helptext = 
        "Help on Serial subsystem\r\n"
        "\nwriting output\r\n"
        "  print=text ... as is\r\n"
        "  println=text ... adding configured line termination\r\n"
        "\nreading input\r\n"
        "  read ... return all characters available in buffer\r\n"
        "\nconfiguration\r\n"
        "  begin=baudrate\r\n"
        "  baud=baudrate\r\n"
        "  rxPin=pin\r\n"
        "  txPin=pin\r\n"
        "  setTerm=CR|LF|CRLF\r\n"
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
    while (((pos=rxBuffer.indexOf(_endOfLine))<0) && (_serial.available()))
    {   // TODO make more efficient
        _serial.read(&c, 1);
        rxBuffer += c;
    }
    if (pos<0) 
        return "";
    String s = rxBuffer.substring(0, pos); // all up to configured end of line 
    rxBuffer = rxBuffer.substring(pos + _endOfLine.length()); // remove part read
    return s;
}

String espSerial::parse(String command, String value)
{
    // _serial.println(command + ":" + value);
    String result = "";
    // split pinList into numbers separated by comma
    // for each number call function()
    // assemble returned strings to JSON like array
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
    else if (command == "print")
        print(value);
    else if (command == "println")
        println(value);
    else if (command == "read")
        result = read();
    else if (command == "readLn")
        result = readln();
    else
        result = "invalid keyword " + command;
    return result;
}

// ESP32 has 3 async serial interfaces 
espSerial webSerial0(Serial);  // pins TX=GPIO0, RX=GPIO1 - connected to USB
espSerial webSerial1(Serial1); // not available on Dev Kit C without remapping pins
espSerial webSerial2(Serial2); // pins TX=GPIO17, RX=GPIO16
