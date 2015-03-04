/*
* v2Parser.h
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include "helper_3dmath.h"

#ifndef V2PARSER_H
#define V2PARSER_H

class v2Parser
{
public:
    v2Parser( int port = 51717 );
    bool setup( int portNum );
    bool recieve();
    void send();

    bool motorsEnabled;
    int motorValues[ 4 ];
    int controlValues[ 6 ];
    Quaternion q;
private:
    int sockfd, newsockfd;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
};

#endif