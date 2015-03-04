// For use with v1 GroundStation

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include "fcntl.h"


#include "wiringPi.h"
#include "PID.h"
#include "PCA9685.h"
#include "MPU6050_6Axis_MotionApps20.h"

// TCP stuff
#define TCP_PORT 51717
int sockfd, newsockfd;
char buffer[256];

// IMU stuff
//uint16_t packetSize;
int packetSize;
MPU6050 mpu;

// servo control stuff
#define MOTOR_OFF 200
#define MOTOR_MIN 220
#define MOTOR_MAX 600
const int MOTOR_ON_RANGE = MOTOR_MAX - MOTOR_MIN;
#define SERVO_FREQ 60
PCA9685 servo;

// PID stuff
#define P_GAIN 2
#define YAW_P_GAIN 0.0001
#define SIN_45 0.70710678118
PID pitchPID, rollPID, yawPID;
int throttle = (int)( ( MOTOR_MAX - MOTOR_MIN ) / 2 + MOTOR_MIN );

void setupTCP( int portno )
{
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    std::cout << "setting up TCP on port " << portno << std::endl;
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
        std::cerr << "ERROR opening socket" << std::endl;

    bzero( (char*) &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    std::cout << "binding socket to portno" << std::endl;
    if( bind( sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) )
        std::cerr << "ERROR binding socket to portno" << std::endl;

    std::cout << "listening to socket" << std::endl;
    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );

    std::cout << "waiting for connection..." << std::endl;
    newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
    if( newsockfd < 0 )
        std::cerr << "ERROR on accept" << std::endl;

    std::cout << "connection established" << std::endl;
    bzero( buffer, 256 );
    fcntl( newsockfd, F_SETFL, O_NONBLOCK );
}

void setupPCA()
{
    servo = PCA9685( 0x40 );
    delay( 5 );
    servo.setFrequency( SERVO_FREQ );
    delay( 5 );
    servo.setPwm( 0, 0, MOTOR_OFF );
    servo.setPwm( 1, 0, MOTOR_OFF );
    servo.setPwm( 2, 0, MOTOR_OFF );
    servo.setPwm( 3, 0, MOTOR_OFF );
    delay( 500 );
    servo.setPwm( 0, 0, MOTOR_OFF );
    servo.setPwm( 1, 0, MOTOR_OFF );
    servo.setPwm( 2, 0, MOTOR_OFF );
    servo.setPwm( 3, 0, MOTOR_OFF );
    delay( 5 );
}

/**
 * returns -1 on failure, 0 on success
 */
int setupMPU()
{
    uint8_t devStatus;

    mpu = MPU6050();

    std::cout << "initializing MPU6050" << std::endl;
    mpu.initialize();

    std::cout << "testing MPU6050 connection..." << std::endl;
    if( mpu.testConnection() )
    {
        std::cout << "MPU6050 connection successful" << std::endl;
    }
    else
    {
        std::cerr << "MPU6050 connection failed" << std::endl;
        return -1;
    }

    std::cout << "initializing DMP" << std::endl;
    devStatus = mpu.dmpInitialize();
    if( devStatus == 0 )
    {
        std::cout << "DMP initialized" << std::endl;

        std::cout << "Enabling DMP" << std::endl;
        mpu.setDMPEnabled( true );

        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        std::cerr << "DMP initialization failed" << std::endl;
        return -1;
    }
    return 0;
}

void setupPID()
{
    pitchPID = PID( P_GAIN, 0, 0 );
    rollPID = PID( P_GAIN, 0, 0 );
    yawPID = PID( YAW_P_GAIN, 0, 0 );
}

void setup()
{
    setupPCA();
    setupTCP( TCP_PORT );
    setupMPU();
    setupPID();
}

void loop()
{
    int fifoCount;
    int nread;
    uint8_t fifoBuffer[64];
    Quaternion q;
    VectorFloat gravity;

    float pitchAngle;
    float rollAngle;
    int16_t yawRate;
    
    float pitchMod = 1;
    float rollMod = 1;
    float yawMod = 1;
    int m0, m1, m2, m3;

    // Motor layout
    // front
    // 3   1
    //  \ /
    //   X
    //  / \
    // 2   0
    // CW:  0, 3
    // CCW: 1, 2

    nread = read( newsockfd, buffer, 255 );
    if( nread > 0 )
    {
        if( strncmp( buffer, "DISCONNECT", strlen("DISCONNECT") ) == 0 )
        {
            std::cout << "SHUTTING DOWN TCP SERVER" << std::endl;

            servo.setPwm( 0, 0, MOTOR_OFF );
            servo.setPwm( 1, 0, MOTOR_OFF );
            servo.setPwm( 2, 0, MOTOR_OFF );
            servo.setPwm( 3, 0, MOTOR_OFF );

            std::cout << "\t0\t0\t0\t0" << std::endl;

            close( newsockfd );
            close( sockfd );

            exit( EXIT_SUCCESS );
        }
        if( strncmp( buffer, "SET_MOTOR_", strlen( "SET_MOTOR_" ) ) == 0 )
        {
            throttle = atoi( buffer + 12 );
        }
    }

    fifoCount = mpu.getFIFOCount();
    if( fifoCount == 1024)
    {
        std::cerr << "FIFO overflow" << std::endl;
    }
    while( fifoCount >= 84 )
    {
        mpu.getFIFOBytes( fifoBuffer, packetSize );
        fifoCount = mpu.getFIFOCount();
    }
    if( fifoCount >= 42 )
    {
        mpu.getFIFOBytes( fifoBuffer, packetSize );
        mpu.dmpGetQuaternion( &q, fifoBuffer );
        mpu.dmpGetGravity( &gravity, &q );

        pitchAngle = atan( gravity.x / sqrt( gravity.y * gravity.y + gravity.z * gravity.z) );
        rollAngle = atan( gravity.y / sqrt( gravity.x * gravity.x + gravity.z * gravity.z ) );
        yawRate = mpu.getRotationZ();

        std::cout << "pitch: " << pitchAngle << "\troll: " << rollAngle << "\tyaw rate: " << yawRate << std::endl;
        pitchMod = pitchPID.update( pitchAngle, 0 );
        rollMod = rollPID.update( rollAngle, 0 );
        yawMod = yawPID.update( yawRate, 0 );

        std::cout << "\t" << pitchMod << "\t" << rollMod << "\t" << yawMod;

        m0 = (int)( ( 1 - pitchMod ) * ( 1 - rollMod ) * ( 1 + yawMod ) * throttle );
        m1 = (int)( ( 1 + pitchMod ) * ( 1 - rollMod ) * ( 1 - yawMod ) * throttle );
        m2 = (int)( ( 1 - pitchMod ) * ( 1 + rollMod ) * ( 1 - yawMod ) * throttle );
        m3 = (int)( ( 1 + pitchMod ) * ( 1 + rollMod ) * ( 1 + yawMod ) * throttle );

        if( m0 < MOTOR_MIN ) m0 = MOTOR_MIN;
        if( m0 > MOTOR_MAX ) m0 = MOTOR_MAX;
        if( m1 < MOTOR_MIN ) m1 = MOTOR_MIN;
        if( m1 > MOTOR_MAX ) m1 = MOTOR_MAX;
        if( m2 < MOTOR_MIN ) m2 = MOTOR_MIN;
        if( m2 > MOTOR_MAX ) m2 = MOTOR_MAX;
        if( m3 < MOTOR_MIN ) m3 = MOTOR_MIN;
        if( m3 > MOTOR_MAX ) m3 = MOTOR_MAX;

        std::cout << "\t" << m0 << "\t" << m1 << "\t" << m2 << "\t" << m3 << std::endl;
        servo.setPwm( 0, 0, m0 );
        servo.setPwm( 1, 0, m1 );
        servo.setPwm( 2, 0, m2 );
        servo.setPwm( 3, 0, m3 );
    }
}

int main( int argc, char* argv[] )
{
    setup();
    while( true )
    {
        loop();
    }
}