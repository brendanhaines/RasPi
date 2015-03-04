/*
* v2main.cpp
*/

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include "v2Parser.h"
#include "PCA9685.h"
#include "DSM2.h"
#include "PID.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "wiringPi.h"

using namespace std;

#define DEFAULT_TCP_PORT 51717

#define MOTOR_OFF 200
#define MOTOR_MIN 220
#define MOTOR_MAX 600

double pitchP   = 2;
double pitchI   = 0;
double pitchD   = 0;
double rollP    = 2;
double rollI    = 0;
double rollD    = 0;
double yawP     = 0.0001;
double yawI     = 0;
double yawD     = 0;

v2Parser tcp;
PCA9685 pca;
DSM2 dsm;
MPU6050 mpu;

PID pitchPID;
PID rollPID;
PID yawPID;

VectorFloat gravity;
int fifoPacketSize;

void setAllMotorsOff()
{
    int i;
    for( i = 0; i < 16; i++ )
        pca.setPwm( i, 0, 0 );
}

void setup( int tcpPort )
{
    cout << "setting up PCA9685 (motor controller)..." << endl;
        pca = PCA9685( 0x40 );
        pca.setFrequency( 60 );
        setAllMotorsOff();
        delay( 250 );
        setAllMotorsOff();
        cout << "motors set to " << MOTOR_OFF << " (OFF)" << endl;
    cout << "PCA9685 set up" << endl;

    cout << "setting up TCP link..." << endl;
        tcp = v2Parser( tcpPort );
    cout << "TCP set up" << endl;

    cout << "setting up RC receiver..." << endl;
        dsm = DSM2();
        dsm.values = tcp.controlValues;
    cout << "RC receiver set up" << endl;

    cout << "setting up MPU6050..." << endl;
        cout << "\tinitializing MPU6050..." << endl;
        mpu.initialize();
        cout << "\ttesting MPU6050 connection..." << flush;
        if( mpu.testConnection() )
        {
            cout << "SUCCESS" << endl;
        }
        else
        {
            cout << "FAILURE" << endl;
            exit( EXIT_FAILURE );
        }
        cout << "\tinitializing DMP..." << flush;
        if( mpu.dmpInitialize() == 0 )
        {
            cout << "\tSUCCESS" << endl;
            cout << "\tEnabling DMP..." << endl;
            mpu.setDMPEnabled( true );
            fifoPacketSize = mpu.dmpGetFIFOPacketSize();
        }
        else
        {
            cout << "\tFAILURE" << endl;
            exit( EXIT_FAILURE );
        }
    cout << "MPU6050 set up" << endl;

    cout << "setting up PID..." << endl;
        pitchPID = PID( pitchP, pitchI, pitchD );
        rollPID = PID( rollP, rollI, rollD );
        yawPID = PID( yawP, yawI, yawD );
    cout << "PID set up" << endl;

    atexit( setAllMotorsOff );
}

bool updateMpu()
{
    int fifoCount;
    uint8_t fifoBuffer[64];
    fifoCount = mpu.getFIFOCount();
    if( fifoCount == 1024 )
    {
        cout << "FIFO overflow" << endl;
    }
    else if( fifoCount >= 42 )
    {
        while( fifoCount >= 84 )
        {
            mpu.getFIFOBytes( fifoBuffer, fifoPacketSize );
            fifoCount = mpu.getFIFOCount();
        }

        mpu.getFIFOBytes( fifoBuffer, fifoPacketSize );
        mpu.dmpGetQuaternion( &(tcp.q), fifoBuffer );
        mpu.dmpGetGravity( &gravity, &(tcp.q) );

        return true;
    }
    return false;
}

void constrainValue( int* in, int low, int high )
{
    if( *in < low )
    {
        *in = low;
    }
    else if( *in > high )
    {
        *in = high;
    }
}

int main( int argc, char* argv[] )
{
    float pitchAngle;
    float rollAngle;
    int16_t yawRate;

    float pitchMod = 1;
    float rollMod = 1;
    float yawMod = 1;

    if( argc < 1 )
    {
        setup( DEFAULT_TCP_PORT );
    }
    else
    {
        setup( *argv[0] );
    }

    //dsm.sync();
    while( true )
    {
        //dsm.update();
        tcp.send();
        tcp.recieve();
        if( updateMpu() )
        {
            pitchAngle = atan( gravity.x / sqrt( gravity.y * gravity.y + gravity.z * gravity.z) );
            rollAngle = atan( gravity.y / sqrt( gravity.x * gravity.x + gravity.z * gravity.z ) );
            yawRate = mpu.getRotationZ();
            cout << "pitch: " << pitchAngle << "\troll: " << rollAngle << "\tyaw rate: " << yawRate << endl;

            pitchMod = pitchPID.update( pitchAngle, 0 );
            rollMod = rollPID.update( rollAngle, 0 );
            yawMod = yawPID.update( yawRate, 0 );

            if( tcp.motorsEnabled )
            {
                tcp.motorValues[ 0 ] = (int)( ( 1 - pitchMod ) * ( 1 - rollMod ) * ( 1 + yawMod ) * tcp.controlValues[0] );
                tcp.motorValues[ 1 ] = (int)( ( 1 + pitchMod ) * ( 1 - rollMod ) * ( 1 - yawMod ) * tcp.controlValues[0] );
                tcp.motorValues[ 2 ] = (int)( ( 1 - pitchMod ) * ( 1 + rollMod ) * ( 1 - yawMod ) * tcp.controlValues[0] );
                tcp.motorValues[ 3 ] = (int)( ( 1 + pitchMod ) * ( 1 + rollMod ) * ( 1 + yawMod ) * tcp.controlValues[0] );

                constrainValue( &(tcp.motorValues[ 0 ]), MOTOR_MIN, MOTOR_MAX );
                constrainValue( &(tcp.motorValues[ 1 ]), MOTOR_MIN, MOTOR_MAX );
                constrainValue( &(tcp.motorValues[ 2 ]), MOTOR_MIN, MOTOR_MAX );
                constrainValue( &(tcp.motorValues[ 3 ]), MOTOR_MIN, MOTOR_MAX );

                pca.setPwm( 0, 0, tcp.motorValues[ 0 ] );
                pca.setPwm( 1, 0, tcp.motorValues[ 1 ] );
                pca.setPwm( 2, 0, tcp.motorValues[ 2 ] );
                pca.setPwm( 3, 0, tcp.motorValues[ 3 ] );
            }
            else
            {
                setAllMotorsOff();
            }
        }
    }
}








