#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include "fcntl.h"

#include "PCA9685.h"
#include "DSM2.h"
#include "PID.h"
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

// PCA9685 stuff
PCA9685 pca;
#define MOTOR_OFF 200
#define MOTOR_MIN 220
#define MOTOR_MAX 600

int motorValues[4];
bool motorsEnabled;
bool motorTesting;

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

void updatePID()
{}

// main
int main(int argc, char const *argv[])
{
    int i;

    setupTCP( TCP_PORT );
    setupPCA();

    // label lines
    cout << "\n**REAL TIME DATA**" << endl;
    cout << "\nRC RECEIVER\t\t\t\t\tMOTORS" << endl;
    cout << "THRO\tAILE\tELEV\tRUDD\tGEAR\tAUX1\tENABLE\tTEST\tM0  \tM1  \tM2  \tM3" << endl;
    while( true )
    {
        dsm.update();
        readTCP();

        if( motorsEnabled && !motorTesting )
            updatePID();
        
        for( i = 0; i < 4; i++ )
            pca.setPwm( i, 0, motorValues[i] );

        cout << "\r" << dsm.values[0] << "  \t" << dsm.values[1] << "  \t" << dsm.values[2] << "  \t" << dsm.values[3] << "  \t" << dsm.values[4] << "  \t" << dsm.values[5] << "  \t" << motorsEnabled << "  \t" << motorTesting << "  \t" << motorValues[0] << "  \t" << motorValues[1] << "  \t" << motorValues[2] << "  \t" << motorValues[3] << "  " << flush;
        delay(100);
    }

    return 0;
}










