#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include "fcntl.h"

#include "pid.h"
#include "PCA9685.h"
#include "MPU6050_6Axis_MotionApps20.h"

// TCP stuff
int sockfd, newsockfd;
char buffer[256];

// IMU stuff
//uint16_t packetSize;
int packetSize;
MPU6050 mpu;

// servo control stuff
#define MOTOR_MIN 200
#define MOTOR_MAX 600
#define SERVO_FREQ 60
PCA9685 servo;

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
    servo = PCA9685();
    servo.setFrequency( SERVO_FREQ );
    servo.setPwm( 0, 0, MOTOR_MIN );
    servo.setPwm( 1, 0, MOTOR_MIN );
    servo.setPwm( 2, 0, MOTOR_MIN );
    servo.setPwm( 3, 0, MOTOR_MIN );
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

void setup()
{
    setupPCA();
    setupTCP( 51717 );
    setupMPU();
}

void loop()
{
    int fifoCount;
    int nread, nwrite;
    uint8_t fifoBuffer[64];
    Quaternion q;
    VectorFloat gravity;
    float ypr[3];

    nread = read( newsockfd, buffer, 255 );
    if( nread > 0 )
    {
        if( strncmp( buffer, "DISCONNECT", strlen("DISCONNECT") ) == 0 )
        {
            std::cout << "SHUTTING DOWN TCP SERVER" << std::endl;

            servo.setPwm( 0, 0, MOTOR_MIN );
            servo.setPwm( 1, 0, MOTOR_MIN );
            servo.setPwm( 2, 0, MOTOR_MIN );
            servo.setPwm( 3, 0, MOTOR_MIN );

            close( newsockfd );
            close( sockfd );
        }
    }

    fifoCount = mpu.getFIFOCount();
    if( fifoCount == 1024)
    {
        std::cerr << "FIFO overflow" << std::endl;
    }
    if( fifoCount >= 42 )
    {
        mpu.getFIFOBytes( fifoBuffer, packetSize );
        mpu.dmpGetQuaternion( &q, fifoBuffer );
        mpu.dmpGetGravity( &gravity, &q );
        mpu.dmpGetYawPitchRoll( ypr, &q, &gravity );

        std::cout << "pitch: " << ypr[1] << std::endl;
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