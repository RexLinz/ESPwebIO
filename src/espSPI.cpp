#include "webIO.h"

const String espSPI::help()
{
    return 
        "Help on SPI subsystem /HSPI and /VSPI = /SPI\r\n"
        "NOTE: FSPI (interfacing ESP32's to flash memory) is prepared but not enabled by default\r\n"
        "\ngeneral settings\r\n"
        "  freqency=Hz ... set frequency (default 1000000)\r\n"
        "  order=MSBFIRST(default)|LSBFIRST ... set bit order\r\n"
        "  mode=0(default)|1|2|3 ... set clock edge and data phase\r\n"
        "  pins=sck,miso,mosi,ss ... set pins to use\r\n"
        "       HSPI defaults to sck=14, miso=12, mosi=13, ss=15\r\n"
        "       VSPI defaults to sck=18, miso=19, mosi=23, ss=5\r\n"
        "  begin=HardwareSS ... open interface\r\n"
        "       if argument HardwareSS is given, ss pin will be controlled by driver\r\n"
        "  end ... end interface\r\n"
        "\nsending and receiving data\r\n"
        "  write=hex,... write out to slave\r\n"
        "  writeread=hex,... write out to slave, return values read as hex value\r\n"
        "  read=num ... read num bytes, return as hex values";
}

String espSPI::setFrequency(String &args)
{
    if (args.length()==0)
        return "\"missing value\"";
    else 
        spiConfig._clock = nextInt(args);
    return String(spiConfig._clock);
}

String espSPI::setBitOrder(const String &args)
{
    if (args.length()==0)
        return String("\"missing value\"");
    else if (args == "MSBFIRST")
        spiConfig._bitOrder = MSBFIRST;
    else if (args == "LSBFIRST")
        spiConfig._bitOrder = LSBFIRST;
    else 
        return "\"invalid\"";
    return String(args);
}

String espSPI::setSPImode(const String &args)
{
    if (args.length()==0)
        return String("\"missing value\"");
    else if (args == "MODE0")
        spiConfig._dataMode = SPI_MODE0;
    else if (args == "MODE1")
        spiConfig._dataMode = SPI_MODE1;
    else if (args == "MODE2")
        spiConfig._dataMode = SPI_MODE2;
    else if (args == "MODE3")
        spiConfig._dataMode = SPI_MODE3;
    else 
        return "\"invalid\"";
    return String(args);
}

String espSPI::setPins(String &args)
{
    if (args.length()==0)
        sckPin = -1; // try to use default
    else 
        sckPin = nextInt(args);
    if (args.length()==0)
        misoPin = -1; // try to use default
    else
        misoPin = nextInt(args);
    if (args.length()==0)
        mosiPin = -1; // try to use default
    else
        mosiPin = nextInt(args);
    if (args.length()==0)
        ssPin = -1; // try to use default
    else
        ssPin = nextInt(args);
    return String(sckPin) + "," + String(misoPin) + "," + String(mosiPin) + "," + String(ssPin);
}

String espSPI::begin(const String &args)
{
    bus.begin(sckPin, misoPin, mosiPin, ssPin);
    bool useHwCs = (args=="HardwareSS");
    bus.setHwCs(useHwCs);
    if (useHwCs)
        return "\"Hardware Slave Select\"";
    else
        return "\"User Slave Select\"";
}

String espSPI::end()
{
    bus.end();
    return "\"done\"";
}

String espSPI::write(String &hexArgs)
{
    bus.beginTransaction(spiConfig);
    int bytesDone = 0;
    while (hexArgs.length() > 0)
    {
        uint8_t n = nextHex(hexArgs);
        bus.transfer(n); // write and ignore response
        bytesDone++;
    }
    bus.endTransaction();
    return String(bytesDone);
}

String espSPI::writeread(String &hexArgs)
{
    String response = "";
    bus.beginTransaction(spiConfig);
    while (hexArgs.length() > 0)
    {
        uint8_t n = nextHex(hexArgs);
        uint8_t b = bus.transfer(n); // write and read response
        addResponse(response, String(b,HEX), ",");
    }
    bus.endTransaction();
    return "\""+response+"\""; // as string, values are hexadecimal!
}

String espSPI::read(const String &numBytes)
{
    String response = "";
    int bytes = numBytes.toInt(); // 
    bus.beginTransaction(spiConfig);
    for (int i=0; i<bytes; i++)
    {
        uint8_t b = bus.transfer(0); // dummy output data
        addResponse(response, String(b,HEX), ",");
    }
    bus.endTransaction();
    return "\""+response+"\""; // as string, values are hexadecimal!
}

String espSPI::parse(const String &command, String value)
{
    String result = "";
    if (command == "pins")
        result = setPins(value); // save for use by following commands
    else if (command == "frequency")
        result = setFrequency(value);
    else if (command == "order")
        result = setBitOrder(value);
    else if (command == "mode")
        result = setSPImode(value);
    else if (command == "begin")
        result = begin(value);
    else if (command == "end")
        result = end();
    else if (command == "write")
        result = write(value);
    else if (command == "writeread")
        result = writeread(value);
    else if (command == "read")
        result = read(value); 
    else
        result = "\"invalid keyword\"";
    return result;
}
