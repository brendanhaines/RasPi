/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

// From:
// http://www.linuxhowtos.org/C_C++/socket.htm
// commented for reference

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error( const char *msg )
{
    perror( msg );
    exit( 1 );
}

int main( int argc, char *argv[] )
{
    // VARIABLE DEFINITONS

    // file descriptors
    int sockfd; // returned by socket system call
    int newsockfd;  // returned by accept system call

    int portno; // stores port number (passed as argument)
    socklen_t clilen;   // stores the size of the client address. needed for the accept system call
    char buffer[ 256 ]; // server reads characters from socket connection into buffer

    // sockaddr_in is defined in "netinet/in.h"
    struct sockaddr_in serv_addr;   // serv_addr contains the server address
    struct sockaddr_in cli_addr;    // cli_addr contains the client address

    int n;  // return value for read/write calls (contains the # of characters read/written)
    
    // BEGIN REAL CODE

    // Check that there are enough arguments
    if( argc < 2 )
    {
        fprintf( stderr, "ERROR, no port provided\n" );
        exit( 1 );
    }
    
    // socket system call creates new socket
    // arg0: address domain ( Unix domain [AF_UNIX] or Internet domain [AF_INET] )
    // arg1: socket type ( Stream [SOCK_STREAM] or Datagram [SOCK_DGRAM] )
    // arg2: protocol ( 0 lets system decide best protocol: TCP for streams, UDP for datagrams )
    // returns file descriptor on success
    // returns -1 on failure
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )    // on error: sockfd = -1
        error( "ERROR opening socket" );
    
    // bzero sets all values in a buffer to zero
    // arg0: pointer to buffer
    // arg1: length of buffer
    bzero( (char*) &serv_addr, sizeof( serv_addr ) );   // initializes serv_addr to all zeroes

    // atoi converts a string of digits to an integer
    portno = atoi( argv[ 1 ] );

    serv_addr.sin_family = AF_INET; // sin_family should always be set to symbolic constant AF_INET
    serv_addr.sin_addr.s_addr = INADDR_ANY; // sin_addr.s_addr contains the IP address of the host. INADDR_ANY is a symbolic constant which get this address
    serv_addr.sin_port = htons( portno ); // sin_port contains the port number. htons converts a port number in host byte order to network byte order
    
    // bind system call binds a socket to an address ( address = address of host & port number)
    // arg0: socket file descriptor
    // arg1: pointer to structure of type sockaddr. what is passed is a structure of type sockaddr_in, must be cast to correct type
    // arg2: size of structure &arg1
    // returns 0 on success ?????????
    if( bind( sockfd, ( struct sockaddr* ) &serv_addr, sizeof( serv_addr ) ) < 0 )
        error( "ERROR on binding" );
    
    // listen system call allows program to listen to socket for connections
    // arg0: file descriptor of socket
    // arg1: size of backlog queue (maximum number of connections which can be waiting while handling any specific connection) (standard: 5)
    listen( sockfd, 5 );

    // set client address length to the size of client's address
    clilen = sizeof( cli_addr );

    // accept system call causes the program to stop until a client connects to the server
    // arg0: file descriptor of the socket
    // arg1: pointer for the client address structure
    // arg2: size of structure &arg1
    // returns a file descriptor for the connection on success
    // returns -1 on failure ?????
    newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
    if( newsockfd < 0 )
        error( "ERROR on accept" );
    
    bzero( buffer, 256 );  // clear the receive buffer

    // read system call reads from socket 
    // read will block the program until there is something to read
    // arg0: file descriptor for the connection (NOTE: this is the new one returned by accept)
    // arg1: buffer to put characters into
    // arg2: maximum number of character to read and put in buffer
    // return number of characters read on success
    // return -1 on failure ?????
    n = read( newsockfd, buffer, 255 );
    if ( n < 0 )
        error("ERROR reading from socket");

    // buffer will contain the message now
    printf( "Here is the message: %s\n", buffer );

    // write system call writes to socket
    // arg0: file descriptor for the connection (NOTE: again, this is the new one returned by accept)
    // arg1: pointer to character (char*)
    // arg2: length of &arg1
    // return number of characters written on success
    // return -1 on failure
    n = write( newsockfd, "I got your message", 18 );
    if ( n < 0 )
        error( "ERROR writing to socket" );

    // when socket is no longer needed, close the connection
    close( newsockfd );
    close( sockfd );

    return 0; 
}
