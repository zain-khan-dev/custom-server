#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>


#define PORT "8080"

struct ServerController {
    struct addrinfo curr_host;


    char* get_filename_ext(char *file_path){
        char* token = strtok(file_path, ".");
        char *last_token = token;
        while(token != NULL){
            last_token = token;
            token = strtok(NULL, ".");
        }
        return last_token;
    }

    void strtolower(char *filename){
        for(int i=0;i<sizeof(filename);i++){
            if(filename[i] >= 'A' && filename[i] <= 'Z'){
                filename[i] = filename[i] - 'A' + 'a';
            }
        }
    }


    void send_headers(const char *path, off_t len, int client_socket) {
        char small_case_path[1024];
        char send_buffer[1024];
        strcpy(small_case_path, path);
        strtolower(small_case_path);
        strcpy(send_buffer, "HTTP/1.0 200 OK\r\n");
        send(client_socket, send_buffer, strlen(send_buffer), 0);
        strcpy(send_buffer, "zain_server");
        send(client_socket, send_buffer, strlen(send_buffer), 0);

        /*
        * Check the file extension for certain common types of files
        * on web pages and send the appropriate content-type header.
        * Since extensions can be mixed case like JPG, jpg or Jpg,
        * we turn the extension into lower case before checking.
        * */
        const char *file_ext = get_filename_ext(small_case_path);
        printf("Inside send headers file extension is %s\n", file_ext);
        if (strcmp("jpg", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: image/jpeg\r\n");
        if (strcmp("jpeg", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: image/jpeg\r\n");
        if (strcmp("png", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: image/png\r\n");
        if (strcmp("gif", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: image/gif\r\n");
        if (strcmp("htm", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: text/html\r\n");
        if (strcmp("html", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: text/html\r\n");
        if (strcmp("js", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: application/javascript\r\n");
        if (strcmp("css", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: text/css\r\n");
        if (strcmp("txt", file_ext) == 0)
            strcpy(send_buffer, "Content-Type: text/plain\r\n");
        send(client_socket, send_buffer, strlen(send_buffer), 0);
        /* Send the content-length header, which is the file size in this case. */
        sprintf(send_buffer, "content-length: %ld\r\n", len);
        send(client_socket, send_buffer, strlen(send_buffer), 0);
        /*
        * When the browser sees a '\r\n' sequence in a line on its own,
        * it understands there are no more headers. Content may follow.
        * */
        strcpy(send_buffer, "\r\n");
        send(client_socket, send_buffer, strlen(send_buffer), 0);
    }
    void transfer_file_contents(char *file_path, int client_socket, off_t file_size)
    {
        int fd;
        fd = open(file_path, O_RDONLY);
        ssize_t bytes_sent = sendfile(client_socket, fd, NULL, file_size);
        if (bytes_sent == -1) {
            printf("Error sendinf file\n");
            perror("Error sending file");
        }
        close(fd);
    }

    void handle_get_method(char *path, int client_socket){
        char final_path[1024];
        printf("Updated Path is %s\n", path);
        if(path[strlen(path) - 1] == '/'){
            strcpy(final_path, "public");
            strcat(final_path, path);
            strcat(final_path, "index.html");
        }
        else{
            strcpy(final_path, "public");
            strcat(final_path, path);
        }
        struct stat path_stat;
        if(stat(final_path, &path_stat) == -1){
            printf("404 Not Found: %s\n", final_path);
            // handle_http_404(client_socket);
        }
        else
        {
            /* Check if this is a normal/regular file and not a directory or something else */
            if (S_ISREG(path_stat.st_mode))
            {

                send_headers(final_path, path_stat.st_size, client_socket);
                transfer_file_contents(final_path, client_socket, path_stat.st_size);
                printf("200 %s %ld bytes\n", final_path, path_stat.st_size);
            }
            else
            {
                // handle_http_404(client_socket);
                printf("404 Not Found: %s\n", final_path);
            }
        }
    }

    void handle_post_method(){

    }
    
    ServerController() {
        memset(&this->curr_host, 0, sizeof(this->curr_host));
        curr_host.ai_flags = AF_UNSPEC;
        curr_host.ai_socktype = SOCK_STREAM;
    }

    struct addrinfo* get_server_info();

    void receive_client_request(int client_fd){
        char buff[1024];
        ssize_t bytes_received = recv(client_fd, buff, sizeof(buff) - 1, 0);
        buff[bytes_received] = '\0';
        char* first_line = strtok(buff, "\r\n");
        char* method = strtok(first_line, " ");
        char* path = strtok(NULL, " ");
        char* version = strtok(NULL, " ");
        if(strcmp(method, "GET") == 0){
            handle_get_method(path, client_fd);
            printf("It is a get method");
        }
        else
        if(strcmp(method, "POST") == 0){
            handle_post_method();
            printf("Handle post method", method, path);
        }
        else{
            printf("Unhandled method", method, path);
        }
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