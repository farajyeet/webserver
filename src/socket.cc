#include "socket.h"


void Socket::closeSocket()
{
    if (DEBUG)
        cout << "Closing socket" << endl;

    fclose(socket_fp);
}


void Socket::acceptClient()
{
    bool interrupted;
    sin_size = sizeof(struct sockaddr_in);

    interrupted = false;

    while (!interrupted) {
        accept_fd = accept(socket_fd, (struct sockaddr *) &client, 
                (socklen_t *)&sin_size);
        if (-1 == accept_fd) {
            if (EINTR == errno) {
                continue; 
            } else {
                perror("Accept");
                interrupted = true;
            }
        } else {
            break;
        }
    }


    if (DEBUG) {
        fprintf(stdout, "Client accepted from %s... my pid is %d\n", inet_ntoa(client.sin_addr), getpid());
    }

    socket_fp = fdopen(accept_fd, "r"); 
}


void Socket::writeLine(string line)
{
    if (send(accept_fd, line.data(), line.size(), 0) == -1) {
        perror("writeLine");
    }	
}


bool Socket::readLine(string *buffer)
{
    char c;
    //    string buffer;

    c = fgetc(socket_fp);

    while ( (c != '\n') && (c != (char)EOF) && (c != '\r') ) {
        *buffer += c;
        c = fgetc(socket_fp);
    }

    if (c == '\n')
        *buffer += c;



    if (c == (char)EOF)
        return false;
    else
        return true;
}



void Socket::serverBind(int server_port)
{
    int yes = 1; 


    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket SFD");
        exit(1);
    }


    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(server_port); 
    server.sin_addr.s_addr = INADDR_ANY;


    if (bind(socket_fd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }


    if (listen(socket_fd, NUM_CLIENTS_TO_QUEUE) == -1) {
        perror("listen");
        exit(1);
    }
}

