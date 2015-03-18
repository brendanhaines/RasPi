#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
//#include <stdint.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include "fcntl.h"

#include "PCA9685.h"
#include "DSM2.h"
#include "PID.h"
//#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "wiringPi.h"

using namespace std;

// TCP vars
#define TCP_PORT 51719
int sockfd, newsockfd;
char buffer[ 256 ];
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;

// DSM2 stuff
const int RX_MAX[]    = { 922, 923, 922, 921, 919, 919 };
const int RX_CENTER[] = { 511, 511, 511, 511, 511, 511 };
const int RX_MIN[]    = { 100, 103, 103, 101, 103, 103 };
DSM2 dsm;   // also initializes

// MPU stuff
MPU6050 mpu;

int packetSize;
int fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
float pitchAngle;
float rollAngle;
int16_t yawRate;

// PCA9685 stuff
PCA9685 pca;
#define MOTOR_OFF 200
#define MOTOR_MIN 220
#define MOTOR_MAX 600

int motorValues[4];
bool motorsEnabled;
bool motorTesting;
int throttle = 300;

// PID stuff
PID pitch, roll, yaw;
double pitchP   = 2;
double pitchI   = 0;
double pitchD   = 0;
double rollP    = 2;
double rollI    = 0;
double rollD    = 0;
double yawP     = 0.0001;
double yawI     = 0;
double yawD     = 0;

// functions
bool setupTCP( int portNum )
{
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    cout << "setting up TCP..." << endl;

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {
        cerr << "ERROR opening socket" << endl;
        return false;
    }

    bzero( (char*) &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNum);

    cout << "binding socket to port " << portNum << endl;
    if( bind( sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) )
    {
        cerr << "ERROR binding socket to port " << portNum << endl;
        return false;
    }

    cout << "listening to socket" << endl;
    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );
    
    cout << "waiting for connection" << endl;
    newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
    if( newsockfd < 0 )
    {
        cerr << "ERROR on accept" << endl;
        return false;
    }

    bzero( buffer, 256 );
    fcntl( newsockfd, F_SETFL, O_NONBLOCK );
    
    cout << "connection established" << endl;
    return true;
}

void setupPCA()
{
    int i;
    pca = PCA9685(0x40);
    pca.setFrequency( 60 );
    for( i = 0; i < 4; i++ )
        pca.setPwm( i, 0, MOTOR_OFF );
    delay(250);
    for( i = 0; i < 4; i++ )
        pca.setPwm( i, 0, MOTOR_OFF );
}

void setupMPU()
{
    mpu = MPU6050();
    cout << "\nsetting up MPU6050..." << endl;
    cout << "initializing MPU6050..." << endl;
    mpu.initialize();
    cout << "testing MPU6050 connection..." << flush;
    if( mpu.testConnection() )
    {
        cout << "SUCCESS" << endl;
    }
    else
    {
        cout << "FAILURE" << endl;
        exit( EXIT_FAILURE );
    }
    cout << "initializing DMP..." << flush;
    if( mpu.dmpInitialize() == 0 )
    {
        cout << "SUCCESS" << endl;
        cout << "Enabling DMP..." << endl;
        mpu.setDMPEnabled( true );
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        cout << "FAILURE" << endl;
        exit( EXIT_FAILURE );
    }
    cout << "MPU6050 set up" << endl;
}

void setupPID()
{
    pitch = PID( pitchP, pitchI, pitchD );
    roll = PID( rollP, rollI, rollD );
    yaw = PID( yawP, yawI, yawD );
}

void readTCP()
{
    uint8_t n;  // number of bytes read
    uint8_t carotPos;
    n = read( newsockfd, buffer, 255 );
    if( n > 0 )
    {
        motorsEnabled = false;
        motorTesting = false;
        carotPos = 0;
        while( carotPos < n )
        {
            if( strncmp( buffer + carotPos, "DISCONNECT", strlen("DISCONNECT") ) == 0 && strlen("DISCONNECT") + carotPos <= n )
            {
                cout << endl;
                exit( EXIT_SUCCESS );
            }
            else if( strncmp( buffer + carotPos, "D", strlen("D") ) == 0 && strlen("D") + carotPos <= n )
            {
                cout << endl;
                exit( EXIT_SUCCESS );
            }
            else if( strncmp( buffer + carotPos, "H", strlen("H") ) == 0 && strlen("H") + carotPos <= n ) carotPos++;
            else if( strncmp( buffer + carotPos, "E", strlen("E") ) == 0 && strlen("E") + carotPos <= n )
            {
                motorsEnabled = true;
                carotPos++;
            }
            else if( strncmp( buffer + carotPos, "M", strlen("M") ) == 0 && strlen("Mxx_xxxx") + carotPos <= n )
            {
                motorsEnabled = true;
                motorTesting = true;
                //cout << atoi( buffer + carotPos + 1 ) << "\t" << atoi( buffer + carotPos + 4 ) << endl;
                motorValues[ atoi( buffer + carotPos + 1 ) ] = atoi( buffer + carotPos + 4 ) * (MOTOR_MAX - MOTOR_OFF) / 100 + MOTOR_OFF;
                carotPos += strlen("Mxx_xxxx");
            }
            else carotPos++;
        }
        bzero( buffer, 256 );
        if( !motorsEnabled )
        {
            motorValues[ 0 ] = MOTOR_OFF;
            motorValues[ 1 ] = MOTOR_OFF;
            motorValues[ 2 ] = MOTOR_OFF;
            motorValues[ 3 ] = MOTOR_OFF;
        }
    }
    return;
}

bool updateMPU()
{
    fifoCount = mpu.getFIFOCount();
    if( fifoCount == 1024 )
    {
        mpu.resetFIFO();
        cout << "FIFO overflow" << endl;
    }
    else if( fifoCount >= 42 )
    {
        while( fifoCount >= 84 )
        {
            mpu.getFIFOBytes( fifoBuffer, packetSize );
            fifoCount = mpu.getFIFOCount();
        }

        mpu.getFIFOBytes( fifoBuffer, packetSize );
        mpu.dmpGetQuaternion( &q, fifoBuffer );
        mpu.dmpGetGravity( &gravity, &q );

        pitchAngle = atan( gravity.x / sqrt( gravity.y * gravity.y + gravity.z * gravity.z) );
        rollAngle = atan( gravity.y / sqrt( gravity.x * gravity.x + gravity.z * gravity.z ) );
        yawRate = mpu.getRotationZ();
        return true;
    }
    return false;
}

void updatePID()
{
    float pitchMod, rollMod, yawMod;

    pitchMod = pitch.update( pitchAngle, 0 );
    rollMod = roll.update( rollAngle, 0 );
    yawMod = yaw.update( yawRate, 0 );

    motorValues[0] = (int)( ( 1 - pitchMod ) * ( 1 - rollMod ) * ( 1 + yawMod ) * throttle );
    motorValues[1] = (int)( ( 1 + pitchMod ) * ( 1 - rollMod ) * ( 1 - yawMod ) * throttle );
    motorValues[2] = (int)( ( 1 - pitchMod ) * ( 1 + rollMod ) * ( 1 - yawMod ) * throttle );
    motorValues[3] = (int)( ( 1 + pitchMod ) * ( 1 + rollMod ) * ( 1 + yawMod ) * throttle );
}

// main
int main(int argc, char const *argv[])
{
    int i;
    bool wasPID = false;

    setupPCA();
    setupTCP( TCP_PORT );
    setupMPU();

    // label lines
    cout << "\n**REAL TIME DATA**\n" << endl;
    cout << "\t";   // FIFO
    cout << "RC RECEIVER\t\t\t\t\t";
    cout << "MOTORS\t\t\t\t\t\t";
    cout << "ORIENTATION\t";
    cout << endl;

    cout << "FIFO\t";
    cout << "THRO\tAILE\tELEV\tRUDD\tGEAR\tAUX1\t";
    cout << "ENABLE\tTEST\tM0  \tM1  \tM2  \tM3\t";
    cout << "PITCH\t\tROLL\t\tYAW\t";
    cout << endl;

    while( true )
    {
        dsm.update();
        readTCP();

        fifoCount = mpu.getFIFOCount();

        updateMPU();

        if( motorsEnabled && !motorTesting )
        {
            if( wasPID )
            {
                updatePID();
            }
            else
            {
                setupPID();
                wasPID = true;
            }
        }
        else
        {
            wasPID = false;
        }
        
        // constrain motor values if enabled: MOTOR_MIN < value < MOTOR_MAX
        if( motorsEnabled && !motorTesting )
        {
            for( i = 0; i < 4; i++ )
            {
                if( motorValues[i] > MOTOR_MAX )
                    motorValues[i] = MOTOR_MAX;
                else if( motorValues[i] < MOTOR_MIN )
                    motorValues[i] = MOTOR_MIN;
            }
        }
        else if( !motorTesting )
        {
            for( i = 0; i < 4; i++ )
                motorValues[i] = MOTOR_OFF;
        }

        for( i = 0; i < 4; i++ )
            pca.setPwm( i, 0, motorValues[i] );

        cout << "\r";
        cout << fifoCount << "\t";
        cout << dsm.values[0] << "  \t" << dsm.values[1] << "  \t" << dsm.values[2] << "  \t" << dsm.values[3] << "  \t" << dsm.values[4] << "  \t" << dsm.values[5] << "\t";
        cout << motorsEnabled << "  \t" << motorTesting << "  \t" << motorValues[0] << "  \t" << motorValues[1] << "  \t" << motorValues[2] << "  \t" << motorValues[3] << "  \t";
        printf( "%+1.5f\t%+1.5f\t%+06d\t", pitchAngle, rollAngle, yawRate );
        cout << flush;
        delay(100);
    }

    return 0;
}










