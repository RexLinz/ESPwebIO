#include "webIO.h"

const String espSPI::help()
{
    return 
        "Help on SPI subsystem TODO /SPI\r\n"
        "\ngeneral settings\r\n"
        "  pins=sck,miso,mosi,ss ... set pins to use\r\n"
        "  freqency=Hz ... set frequency (default 100 kHz, max 400 kHz)\r\n"
        "  begin ... open interface\r\n"
        "  scan ... scan bus and return address(es) of device(s) found\r\n"
        "           last one found will be set as default address\r\n"
        "  end ... end interface\r\n"
        "\nsending and receiving data\r\n"
        "  address=hex ... set read/write address\r\n"
        "  write=hex,... write out to slave, return number of bytes done\r\n"
        "  read=num ... read num bytes, return as hex values";
}

String espSPI::setPins(String args)
{
    if (args.length()==0)
        sckPin = -1;
    else 
        sckPin = nextInt(args);
    if (args.length()==0)
        misoPin = -1;
    else
        misoPin = nextInt(args);
    if (args.length()==0)
        mosiPin = -1;
    else
        mosiPin = nextInt(args);
    if (args.length()==0)
        ssPin = -1;
    else
        ssPin = nextInt(args);
    return String(sckPin) + "," + String(misoPin) + "," + String(mosiPin) + "," + String(ssPin);
}

String espSPI::setFrequency(String args)
{
    if (args.length()==0)
        spiConfig._clock = 0;
    else 
        spiConfig._clock = nextInt(args);
    return String(spiConfig._clock);
}

String espSPI::setBitOrder(String args)
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

String espSPI::setSPImode(String args)
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

String espSPI::begin()
{
    bus.begin(sckPin, misoPin, mosiPin, ssPin);
    return "\"done\"";
}

String espSPI::end()
{
    bus.end();
    return "\"done\"";
}

String espSPI::write(String hexArgs)
{
    String response = "";
    bus.beginTransaction(spiConfig);
    int bytesDone = 0;
    while (hexArgs.length() > 0)
    {
        uint8_t n = nextHex(hexArgs);
        uint8_t b = bus.transfer(n); // write and read response
        addResponse(response, String(b,HEX), ",");
    }
    bus.endTransaction();
    return "\""+response+"\""; // as string, values are hexadecimal!
}

String espSPI::read(String numBytes)
{
    String response = "";
    int bytes = numBytes.toInt(); // 
    bus.beginTransaction(spiConfig);
    for (int i=0; i<bytes; i++)
    {
        uint8_t b = bus.transfer(0); // dummy output data
        addResponse(response, String(b,HEX), ",");
    }
    return "\""+response+"\""; // as string, values are hexadecimal!
}

String espSPI::parse(String command, String value)
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
        result = begin();
    else if (command == "end")
        result = end();
    else if (command == "write")
        result = write(value);
    else if (command == "read")
        result = read(value); 
    else
        result = "\"invalid keyword\"";
    // complete result as JSON
    if (result.length()==0)
        return ""; // no output generated
    if (result.indexOf(",") > 0)
        result =  "[" + result + "]"; // output is array
    return "\"" + command + "\":" + result;
}

espSPI webFSPI(FSPI); // SPI1 attached to flash memory
espSPI webHSPI(HSPI); // SPI2 attached to pins mosi=12, 13, clk=14, ss=15 ???
espSPI webVSPI(VSPI); // SPI3 attached to ss=5, clk=18, miso=19, mosi=23
