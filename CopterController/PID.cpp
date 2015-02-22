/*
* pid.cpp
*/

#include "PID.h"
#include <ctime>

PID::PID( double p, double i, double d )
{
    pGain = p;
    iGain = i;
    dGain = d;

    lastPosition = 0;
    integral = 0;

    lastUpdate = clock();
}

double PID::update( double current, double desired )
{
    double pAdjust, iAdjust, dAdjust;
    double difference;
    double posChange;
    clock_t newTime;
    clock_t deltaT;

    newTime = clock();
    deltaT = newTime - lastUpdate;
    difference = desired - current;

    // Porportional
    pAdjust = pGain * difference;

    // Integral
    integral += difference * deltaT;
    iAdjust = -iGain * integral;

    // Differential
    posChange = current - lastPosition;
    dAdjust = -dGain * posChange / deltaT ;
    

    return (1 + pAdjust) * (1 + iAdjust) * (1 + dAdjust) - 1;
}

