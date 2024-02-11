#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server/socket_connection.h"
#include "server/server_controller.h"


#define PORT "8080"
#define CONCURRENT_CONNECTIONS 10



int main(){
    // Get the addrinfo of the current host
    // make a socket over the host ip address 
    // bind that ip to a specific port
    struct SocketConnection new_socket;
    
    struct ServerController server_controller;


    struct addrinfo* serverinfo = server_controller.get_server_info();

    
    new_socket.create_socket_connection(serverinfo);

    new_socket.listen_for_connections();
    printf("Starting listenting for connections\n");
    server_controller.start_server_loop(new_socket.sockfd);
}