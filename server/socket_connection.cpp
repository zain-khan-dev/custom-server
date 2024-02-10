#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define CONCURRENT_CONNECTIONS 10

struct SocketConnection {
    struct addrinfo curr_host;
    int sockfd;
    SocketConnection(){
        memset(&curr_host, 0, sizeof(curr_host));
        curr_host.ai_flags = AF_UNSPEC;
        curr_host.ai_socktype = SOCK_STREAM;
    }

    struct addrinfo get_curr_host() {
        return curr_host;
    }

    void listen_for_connections();

    void create_socket_connection(struct addrinfo* serverinfo);

};


void SocketConnection::listen_for_connections(){
    if(listen(sockfd, CONCURRENT_CONNECTIONS) == -1){
        perror("Error while listenting to connections");
        exit(0);
    }
}


void SocketConnection::create_socket_connection(struct addrinfo* serverinfo){
    struct addrinfo* p;
    int yes = 1;
    for(p=serverinfo; p!= NULL; p=p->ai_next){
        this->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(this->sockfd == -1){
            perror("Error setuping up socket");
            continue;
        }
        if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        ((struct sockaddr_in *)p)->sin_addr.s_addr = htonl(INADDR_ANY);
        int bind_value = bind(this->sockfd, p->ai_addr, p->ai_addrlen);
        if(bind_value == -1){
            close(this->sockfd);
            continue;
        }
    }
    if(sockfd == -1){
        fprintf(stderr, "Error in getting socket");
        exit(0);
    }
}