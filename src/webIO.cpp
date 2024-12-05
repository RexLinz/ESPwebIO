#include "webIO.h"

// helping functions which might be used by several parsers

// get the next int value from comma separated list, 
// remove that from list
int nextInt(String &list)
{
    int val = list.toInt();
    int n = list.indexOf(',');
    if (n>=0)
        list = list.substring(n+1);
    else
        list = "";
    return val;
}

// get the next float value from comma separated list, 
// remove that from list
float nextFloat(String &list)
{
    float val = list.toFloat();
    int n = list.indexOf(',');
    if (n>=0)
        list = list.substring(n+1);
    else
        list = "";
    return val;
}

// public available objects
espGPIO webGPIO;
