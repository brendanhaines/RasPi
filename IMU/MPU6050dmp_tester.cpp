#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "wiringPi.h"

#define INTERRUPT_PIN 7 // silkscreen: 4
#define LED_PIN 6 // silkscreen: 25
#define SWITCH_PIN // silkscreen: 

MPU6050 mpu; // AD0 pin low
//MPU6050 mpu(0x69) // AD0 pin high

// MPU control/status vars
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
float sensorYaw;
int32_t


// ================================================================
// ===                   INTERRUPT FUNCTION                     ===
// ================================================================

void dmpDataReady()
{
    fifoCount = mpu.getFIFOCount();

    if (fifoCount == 1024)
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        printf("FIFO overflow!\n");
    }
    else if( fifoCount >= 42 )
    {
        // orientation/motion vars
        Quaternion q;           // [w, x, y, z]         quaternion container
        VectorInt16 aa;         // [x, y, z]            accel sensor measurements
        VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
        VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
        VectorFloat gravity;    // [x, y, z]            gravity vector
        int32_t[3] accel;
        //float euler[3];         // [psi, theta, phi]    Euler angle container
        float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
        
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccelInWorld( , , &q );

        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        printf("ypr  %7.2f %7.2f %7.2f    ", ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
        printf( "\n" );
        sensorYaw = ypr[0] * 180/M_PI;
    }
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup()
{
    // setup led as output
    pinMode( LED_PIN, OUTPUT );
   
    printf( "Setting up GPIO...\n" );
    wiringPiSetup();
    pinMode( INTERRUPT_PIN, INPUT );

    printf("Initializing I2C devices...\n");
    mpu.initialize();

    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    printf("Initializing DMP...\n");
    uint8_t devStatus = mpu.dmpInitialize();

    if( devStatus == 0 )
    {
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        printf( "Enabling external interrupt on pin %d\n", INTERRUPT_PIN);
        wiringPiISR( INTERRUPT_PIN, INT_EDGE_RISING, dmpDataReady );

        printf("DMP ready!\n");

        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

int main( int argc, char* argv[] )
{
    setup();
    usleep(100000);
    while(1)
    {}
    return 0;
}
