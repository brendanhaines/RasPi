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

#include "DSM2.h"

using namespace std;

// TCP vars
#define TCP_PORT 51714
int sockfd, newsockfd;
char buffer[ 256 ];
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;

// DSM2 receiver vars
const int RX_MAX[]    = { 900, 900, 900, 900, 900, 900 };
const int RX_CENTER[] = { 511, 511, 511, 511, 511, 511 };
const int RX_MIN[]    = { 122, 122, 122, 122, 122, 122 };
DSM2 dsm;

// Status vars
bool motorsEnabled;
int motorValues[ 4 ];
//Quaternion q;

bool setupTcp( int portNum )
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

void shutdown()
{
    cout << "shutting down..." << endl;

    close( newsockfd );
    close( sockfd );
    cout << "sockets closed" << endl;

    exit( EXIT_SUCCESS );
}

void readTcp()
{
    int n;  // number of bytes read
    int carotPos;
    n = read( newsockfd, buffer, 255 );
    if( n > 0 )
    {
        motorsEnabled = false;
        carotPos = 0;
        while( carotPos < n )
        {
            if( strncmp( buffer + carotPos, "DISCONNECT", strlen("DISCONNECT") ) == 0 && strlen("DISCONNECT") + carotPos <= n ) shutdown();
            else if( strncmp( buffer + carotPos, "D", strlen("D") ) == 0 && strlen("D") + carotPos <= n ) shutdown();
            else if( strncmp( buffer + carotPos, "H", strlen("H") ) == 0 && strlen("H") + carotPos <= n )
            {
                cout << "H " << flush;
                carotPos++;
            }
            else if( strncmp( buffer + carotPos, "E", strlen("E") ) == 0 && strlen("E") + carotPos <= n )
            {
                motorsEnabled = true;
                cout << "E " << flush;
                carotPos++;
//                carotPos += strlen("E");
            }
            else if( strncmp( buffer + carotPos, "M", strlen("M") ) == 0 && strlen("Mxx_xxxx") + carotPos <= n )
            {
                motorValues[ atoi( buffer + carotPos + 1 ) ] = atoi( buffer + carotPos + 4 );
                carotPos += strlen("Mxx_xxxx");
            }
            /*else if( strncmp( buffer + carotPos, "C", strlen("C") ) == 0 && strlen("Cxx_xxxx") + carotPos <= n )
            {
                dsm.values[ atoi( buffer + carotPos + 1 ) ] = atoi( buffer + carotPos + 4 );
                carotPos += strlen("Cxx_xxxx");
            }*/
            else carotPos++;
        }
        cout << endl;
        bzero( buffer, 256 );
        if( !motorsEnabled )
        {
            motorValues[ 0 ] = 0;
            motorValues[ 1 ] = 0;
            motorValues[ 2 ] = 0;
            motorValues[ 3 ] = 0;
        }
    }
}

void writeTcp()
{
    int i;
    char temp[256];
    char outBuffer[] = "H ";

    if( motorsEnabled ) {
        strcat( outBuffer, "E " );

        for( i = 0; i < 4; i++ )
        {
            sprintf( temp, "M%02d_%04d ", i, motorValues[ i ] );
            strcat( outBuffer, temp );
        }
    }

    for( i = 0; i < 6; i++ )
    {
        sprintf( temp, "C%02d_%04d ", i, dsm.values[ i ] );
        strcat( outBuffer, temp );
    }
}

int main( int argc, char* argv[] )
{
    //if( !setupTcp( TCP_PORT ) ) exit( EXIT_FAILURE );
    setupTcp( TCP_PORT );
    while( true )
    {
        readTcp();
        cout << "EN: " << motorsEnabled << "\tM0: " << motorValues[0] << "\tM1: " << motorValues[1] << "\tM2: " << motorValues[2] << "\tM3: " << motorValues[3] << endl;
        writeTcp();

    }
}






