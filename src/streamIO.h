// parsing from stream like serial, TCP or UDP
#ifndef _STREAMIO_H_
#define _STREAMIO_H_
#include <Arduino.h>

// non blocking parsing of input stream
// read full line up to newline
// then parse that as it would be url in httpIO, e.g.
// /GPIO?set=17
// result is written back to stream

class streamIO : private espRoot
{
private:
    String _endOfBlock;
    String rxBuffer;
    String argName;
    String argValue;
    void nextArg(String &args); // extract next argument from list, store in argName and argValue
    String streamJSON(espRoot &base, String args);
    String streamSerial(espSerial &base, String args);
public:
    explicit streamIO(String termination="\004") : _endOfBlock(termination) {}; // 004 = End Of Transmission (EOT)
    void hostTerm(String termination) { _endOfBlock = termination; };
    void parse(Stream &s);
};

extern streamIO webStream;

#endif