// For use with v1 GroundStation

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include "fcntl.h"

#include "wiringPi.h"
#include "PCA9685.h"

#define MOTOR_MIN 200
#define MOTOR_MAX 600
#define SERVO_FREQ 60

int sockfd, newsockfd;
socklen_t clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int nread, nwrite;

void setupTCP( int portno )
{
    printf( "Setting up socket\n" );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 ) 
        printf( "ERROR opening socket" );

    bzero( (char *) &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    printf( "Binding socket to portno\n" );
    if( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) 
            printf("ERROR on binding");

    printf( "Listening\n" );
    listen( sockfd, 5 );
    clilen = sizeof(cli_addr);

    printf( "Waiting for connection\n" );
    newsockfd = accept( sockfd, 
                (struct sockaddr *) &cli_addr, 
                &clilen );
    if( newsockfd < 0 ) 
        printf( "ERROR on accept" );

    printf( "Connected\n" );
    bzero( buffer, 256 );
    fcntl( newsockfd, F_SETFL, O_NONBLOCK );
}

int main( int argc, char* argv[] )
{

    setupTCP( 51717 );

    wiringPiSetup();
    PCA9685 servo = PCA9685( 0x40 );
    servo.setFrequency( SERVO_FREQ );
    servo.setPwm( 0, 0, MOTOR_MIN );
    servo.setPwm( 1, 0, MOTOR_MIN );
    servo.setPwm( 2, 0, MOTOR_MIN );
    servo.setPwm( 3, 0, MOTOR_MIN );

    while( true )
    {
        nread = read( newsockfd, buffer, 255 );
        if( nread < 0 )
        {
            //printf( "ERROR on read" );
        }
        else if( nread > 0 )
        {
            nwrite = write( newsockfd, buffer, strlen(buffer) );
            if( nwrite < 0 )
                printf( "ERROR on write" );
            //PARSE
            printf( "Just read: %s", buffer );
            if( strncmp( buffer, "DISCONNECT", strlen("DISCONNECT") ) == 0 )
            {
                printf( "Closing Socket\n" );
                close( newsockfd );
                close( sockfd );
                return 0;
            }
            else if( strncmp( buffer, "HEARTBEAT", strlen("HEARTBEAT") ) == 0 )   // hearbeat transmission
            {
                // format:
                // HEARTBEAT FCENABLE_x 
                // FCENABLE_x flight controller enable/disable x=0: disabled x=1: enabled
            }
            else if( strncmp( buffer, "SET_MOTOR_", strlen("SET_MOTOR_") ) == 0 )
            {
                int motor = atoi( buffer + 10 );
                int speed = atoi( buffer + 13 );
                if( speed < MOTOR_MIN )
                {
                    speed = MOTOR_MIN;
                }
                else if( speed > MOTOR_MAX )
                {
                    speed = MOTOR_MAX;
                }
                if( motor >= 0 && motor <= 3 )
                {
                    printf( "setting motor %d to %d\n", motor, speed );
                    servo.setPwm( motor, 0, speed );
                    delay(5);
                    printf( "motor %d set to %d\n", motor, speed );
                }
            }
            bzero( buffer, 256 );
            nread  = 0;
        }
        delay(5);
    }
}






