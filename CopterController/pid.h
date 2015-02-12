/*
* pid.h
*/

#ifndef PID_H
#define PID_H

#include <ctime>

class pid
{
public:
    pid( double p, double i, double d );
    
    double update( double current, double desired );
private:
    double pGain, iGain, dGain;

    clock_t lastUpdate;
    double integral, lastPosition;
};

#endif