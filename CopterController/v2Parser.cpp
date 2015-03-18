/*
* v2Parser.cpp
*/

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

#include "v2Parser.h"

using namespace std;

// No-Args constructor
v2Parser::v2Parser()
{}

v2Parser::v2Parser( int port )
{
    setup( port );
}

bool v2Parser::setup( int portNum )
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

bool v2Parser::recieve()
{
    size_t n;  // number of bytes read
    uint8_t carotPos;
    int channel, value;

    n = read( newsockfd, buffer, 255 );
    if( n > 0 )
    {
        motorsEnabled = false;
        carotPos = 0;
        while( carotPos < n )
        {
            if( strncmp( buffer + carotPos, "DISCONNECT", strlen("DISCONNECT") ) == 0 && strlen("DISCONNECT") + carotPos <= n ) exit( EXIT_SUCCESS );
            else if( strncmp( buffer + carotPos, "D", strlen("D") ) == 0 && strlen("D") + carotPos <= n ) exit( EXIT_SUCCESS );
            else if( strncmp( buffer + carotPos, "H", strlen("H") ) == 0 && strlen("H") + carotPos <= n ) carotPos++;
            else if( strncmp( buffer + carotPos, "E", strlen("E") ) == 0 && strlen("E") + carotPos <= n )
            {
                motorsEnabled = true;
                cout << "E ";
                carotPos += strlen("E ");
            }
            else if( strncmp( buffer + carotPos, "M", strlen("M") ) == 0 && strlen("Mxx_xxxx") + carotPos <= n )
            {
                channel = atoi( buffer + carotPos + 1 );
                value = atoi( buffer + carotPos + 4 );
                motorValues[ channel ] = value;
                cout << "M" << channel << endl;
                carotPos += strlen("Mxx_xxxx ");
            }
            /*else if( strncmp( buffer + carotPos, "C", strlen("C") ) == 0 && strlen("Cxx_xxxx") + carotPos <= n )
            {
                channel = atoi( buffer + carotPos + 1 );
                value = atoi( buffer + carotPos + 4 );
                motorValues[ channel ] = value;
                cout << "C" << channel << endl;
                carotPos += strlen("Cxx_xxxx ");
            }*/
            else carotPos++;
        }
        cout << endl;
        return true;
    }
    return false;
}

void v2Parser::send()
{
    int i;
    char temp[256];
    char outBuffer[] = "H ";

    if( motorsEnabled ) {
        strcat( outBuffer, "E " );

        for( i = 0; i < 4; i++ )
        {
            sprintf( temp, "M%2d_%4d ", i, motorValues[ i ] );
            strcat( outBuffer, temp );
        }
    }
}