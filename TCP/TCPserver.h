/*
* TCPserver.h
* Written by Brendan Haines
* Based heavily on example code found at http://www.linuxhowtos.org/C_C++/socket.htm
*/

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>


class TCPserver {
public:
    TCPserver();
    TCPserver( int port ); // Constructor
    ~TCPserver();   // Destructor

    void handleError( const char* message );

    char* getBuffer();
    int getConnectionFD();

    char buffer[ 256 ]; // buffer to store read data
private:
    int socketFD;       // file descriptor for the socket
    int connectionFD;   // file descriptor for the connection
    int portNumber;     // port number of the socket
    socklen_t clientAddrLen;    // stores the length of the client's address

    struct sockaddr_in server_addr; // contains the server's address
    struct sockaddr_in client_addr; // contains the client's address

    //pthread_t monitorThread;    // thread to monitor the socket and read to buffer

    bool setupSocket();

    //void startMonitoringThread();
    //void* monitoringThread( void*  );
};

#endif
