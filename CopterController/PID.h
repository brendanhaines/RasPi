/*
* pid.h
*/

#ifndef PID_H
#define PID_H

#include <ctime>

class PID
{
public:
    PID( double p = 0, double i = 0, double d = 0 );
    
    double update( double current, double desired );
private:
    double pGain, iGain, dGain;

    clock_t lastUpdate;
    double integral, lastPosition;
};

#endif