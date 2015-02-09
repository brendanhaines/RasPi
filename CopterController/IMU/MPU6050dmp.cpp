/*
* MPU6050dmp.cpp
*/

#include "MPU6050dmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050dmp::MPU6050dmp( int devAddr )
{
    uint8_t devStatus;

    printf("Initializing I2C devices...\n");
    mpu.initialize();

    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();

    if (devStatus == 0)
    {
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        mpuIntStatus = mpu.getIntStatus();

        printf("DMP ready!\n");
        dmpReady = true;

        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
}

int update()
{
    if( !dmpReady ) // setup failed
    {
        printf("dmp Initialization failed...cannot read from dmp\n");
        return -1;
    }

    fifoCount = mpu.getFIFOCount();
    if( fifoCount == 1024 )
    {
        mpu.resetFIFO();
        printf( "FIFO overflow\n" );
    }
    else if( fifoCount >= 42 )
    {
        mpu.getFIFOBytes( fifoBuffer, packetSize );
        mpu.dmpGetQuaternion( &q, fifoBuffer );
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity( &gravity, &q );
        mpu.dmpGetLinearAccelInWorld( &aaWorld, &aaReal, &q );
    }
}




