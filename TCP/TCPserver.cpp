/*
* TCPserver.cpp
* Written by Brendan Haines
* Based heavily on example code found at http://www.linuxhowtos.org/C_C++/socket.htm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include "TCPserver.h"

TCPserver::TCPserver()
{}

// Constructor sets everything up and starts monitoring thread
TCPserver::TCPserver( int port )
{
    portNumber = port;
    setupSocket();
}

// Destructor closes sockets
TCPserver::~TCPserver()
{
    close( connectionFD );
    close( socketFD );
}

// a unified/consistent way of reporting errors
void TCPserver::handleError( const char* message )
{
    perror( message );  // print message to stderror

}

// Returns true on success, false on failure
bool TCPserver::setupSocket()
{
    // Create a new socket
    // AF_INET: internet domain (not unix domain)
    // SOCK_STREAM: stream (not datagram)
    // 0: lets OS decide correct protocol (TCP)
    socketFD = socket( AF_INET, SOCK_STREAM, 0 );
    if( socketFD < 0 )  // error setting up socket
    {
        handleError( "Error setting up socket\n" );
        return false;
    }

    // fill server_addr with zeroes
    // NOTE: memset sets arg2 bytes following arg0 to arg1
    // NOTE: sizeof returns the size of the passed type in bytes
    memset( (char*) &server_addr, 0, sizeof( server_addr ) );

    server_addr.sin_family = AF_INET;    // just following the example (and its documentation)
    server_addr.sin_port = htons( portNumber );  // assign the port number. NOTE: htons converts host byte order to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;    // INADDR_ANY represents the IP address of the host

    // bind: binds the socket to an address
    if( bind( socketFD, (struct sockaddr*) &server_addr, sizeof( server_addr ) ) < 0 )
    {
        handleError( "Error binding socket" );
        return false;
    }

    // listen for connections to socket (max 5 waiting)
    listen( socketFD, 5 );

    clientAddrLen = sizeof( client_addr );

    // wait for client to connect
    printf( "Waiting for client to connect\n" );
    connectionFD = accept( socketFD, (struct sockaddr*) &client_addr, &clientAddrLen );
    if( connectionFD < 0 )
    {
        handleError( "Error on accept" );
        return false;
    }

    // empty buffer
    memset( buffer, 0, sizeof( buffer ) );

    return true;    // setup successful
}

// returns a pointer to the buffer
char* TCPserver::getBuffer()
{
    return buffer;
}

// returns the connection file descriptor
int TCPserver::getConnectionFD()
{
    return connectionFD;
}
/*
void TCPserver::startMonitoringThread()
{
    // create thread: ( thread, NULL = default values, function to run, parameter (void*) or NULL )
    if( pthread_create( &monitorThread, NULL, monitoringThread, (void*) this ) )
    {
        // error creating thread
        handleError( "Error creating thread" );
        return;
    }
}

void* TCPserver::monitoringThread( void* serverPointer )
{
    int n;  // contains the number of characters read

    TCPserver server;

    while( true )
    {
        // read from the socket and put in the buffer
        n = read( server.getConnectionFD(), server.getBuffer(), 255 );
        if( n < 0 )
        {
            server.handleError( "Error reading -- monitorThread " );
            pthread_exit( NULL ); // exit this thread
        }
    }
}
*/

