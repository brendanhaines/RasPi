#ifndef MPU6050DMP_H
#define MPU6050DMP_H

#include "MPU6050_6Axis_MotionApps20.h"
#include "helper_3dmath.h"

class MPU6050dmp
{
public:
    MPU6050dmp( int devAddr = 0x68 );

    int update();

    Quaternion q;
    VectorInt16 aa;
    VectorInt16 aaReal;
    VectorInt16 aaWorld;
    VectorFloat gravity;
    int16_t xg;
    int16_t yg;
    int16_t zg;
private:
    MPU6050 mpu;

    bool dmpReady = false;
    uint8_t mpuIntStatus;
    uint16_t packetSize;
    uint16_t fifoCount;
    uint8_t fifoBuffer[ 64 ];
};

#endif