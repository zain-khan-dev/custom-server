#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define PORT "8080"
#define CONCURRENT_CONNECTIONS 10


void *get_in_addr(struct sockaddr* curr_addr){
    if(curr_addr->sa_family == AF_INET){
        return &(((struct sockaddr_in*)(curr_addr))->sin_addr);
    }
    return &(((struct sockaddr_in6*)(curr_addr))->sin6_addr);
}



struct addrinfo* get_server_info(struct addrinfo curr_host){
    int rv;
    struct addrinfo* serverinfo;
    if((rv = getaddrinfo(NULL, PORT, &curr_host, &serverinfo)) == -1){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(0);
    }
    return serverinfo;
}

int create_socket_connection(struct addrinfo* serverinfo){
    int sockfd;
    struct addrinfo* p;
    int yes = 1;
    for(p=serverinfo; p!= NULL; p=p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd == -1){
            perror("Error setuping up socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        ((struct sockaddr_in *)p)->sin_addr.s_addr = htonl(INADDR_ANY);
        int bind_value = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if(bind_value == -1){
            close(sockfd);
            continue;
        }
        return sockfd;
    }
    return -1;
}


void receive_client_request(int fd){
    char buff[1024];
    recv(fd, buff, 1024, 0); 
    printf(buff);
}

void start_server_loop(int sockfd){

    while(1){
        sockaddr_storage *their_addr;
        socklen_t their_addr_size = sizeof their_addr;
        int new_fd = accept(sockfd, (struct sockaddr*)get_in_addr((struct sockaddr*)&(their_addr)), 
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


int main(){
    // Get the addrinfo of the current host
    // make a socket over the host ip address 
    // bind that ip to a specific port
    struct addrinfo curr_host;
    
    memset(&curr_host, 0, sizeof(curr_host));
    curr_host.ai_flags = AF_UNSPEC;
    curr_host.ai_socktype = SOCK_STREAM;

    struct addrinfo* serverinfo = get_server_info(curr_host);
    int sockfd = create_socket_connection(serverinfo);
    if(sockfd == -1){
        fprintf(stderr, "Error in getting socket");
        exit(0);
    }
    
    if(listen(sockfd, CONCURRENT_CONNECTIONS) == -1){
        perror("Error while listenting to connections");
    }
    printf("Starting listenting for connections\n");
    start_server_loop(sockfd);
}