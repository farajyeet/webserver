#ifndef SHELOB_SOCKET_H
#define SHELOB_SOCKET_H 1

#include <cerrno>
#include <cstdio>
#include <string>
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "global.h"


class Socket {

private:
    int socket_fd, sin_size;    
    struct sockaddr_in server;     
    const static int NUM_CLIENTS_TO_QUEUE = 10; 
    void serverBind(int server_port);
    FILE *socket_fp;    
  
public: 
 
    int accept_fd, pid;

    struct sockaddr_in client;

    void acceptClient();
    bool readLine(string *buffer);
    void writeLine(string line);
    void closeSocket();


    Socket(int server_port) {

		serverBind(server_port);	
    }
};

#endif 
