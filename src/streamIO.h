// parsing from stream like serial, TCP or UDP
#ifndef _STREAMIO_H_
#define _STREAMIO_H_
#include <Arduino.h>

// non blocking parsing of input stream
// read full line up to newline
// then parse that as it would be url in httpIO, e.g.
// /GPIO?set=17
// result is written back to stream

class streamIO : private espUtil
{
private:
    String _endOfLine;
    String rxBuffer;
    String streamIO::GPIO(String args);
public:
    streamIO(String termination="\r\n") : _endOfLine(termination) {};
    void setTerm(String termination) { _endOfLine = termination; };
    void parse(Stream &s);
};

extern streamIO webStream;

#endif