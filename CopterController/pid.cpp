/*
* pid.cpp
*/

#include "pid.h"
#include <ctime>

pid::pid( double p, double i, double d )
{
    pGain = p;
    iGain = i;
    dGain = d;

    lastPosition = 0;
    integral = 0;

    lastUpdate = clock();
}

double pid::update( double current, double desired )
{
    double pAdjust, iAdjust, dAdjust;
    double difference;
    double posChange;
    clock_t newTime;

    difference = desired - current;

    // p
    pAdjust = pGain * difference;

    // i
    newTime = clock();
    integral += difference * (newTime - lastUpdate);
    iAdjust = -iGain * integral;
    lastUpdate = newTime;

    // d
    posChange = current - lastPosition;
    dAdjust = -dGain * posChange;
    

    return pAdjust + iAdjust + dAdjust;
}

