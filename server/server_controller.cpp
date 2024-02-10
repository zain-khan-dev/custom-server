#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define PORT "8080"

struct ServerController {
    struct addrinfo curr_host;
    
    ServerController() {
        memset(&this->curr_host, 0, sizeof(this->curr_host));
        curr_host.ai_flags = AF_UNSPEC;
        curr_host.ai_socktype = SOCK_STREAM;
    }

    struct addrinfo* get_server_info();

    void receive_client_request(int client_fd){
        char buff[1024];
        recv(client_fd, buff, 1024, 0); 
        printf(buff);
    }

    void *get_in_addr(struct sockaddr* curr_addr){
        if(curr_addr->sa_family == AF_INET){
            return &(((struct sockaddr_in*)(curr_addr))->sin_addr);
        }
        return &(((struct sockaddr_in6*)(curr_addr))->sin6_addr);
    }

    void start_server_loop(int sockfd);
};



struct addrinfo* ServerController::get_server_info(){
    int rv;
    struct addrinfo* serverinfo;
    if((rv = getaddrinfo(NULL, PORT, &this->curr_host, &serverinfo)) == -1){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(0);
    }
    return serverinfo;
}

void ServerController::start_server_loop(int sockfd){

    while(1){
        sockaddr_storage *their_addr;
        socklen_t their_addr_size = sizeof their_addr;
        int new_fd = accept(sockfd, (struct sockaddr*)this->get_in_addr((struct sockaddr*)&(their_addr)), 
        &their_addr_size);
        
        if(!fork()){
            receive_client_request(new_fd);
            send(new_fd, "Helloworld", 9, 0);
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }
}