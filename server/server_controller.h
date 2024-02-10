struct ServerController {
    struct addrinfo curr_host;
    ServerController() {
        memset(&this->curr_host, 0, sizeof(this->curr_host));
        curr_host.ai_flags = AF_UNSPEC;
        curr_host.ai_socktype = SOCK_STREAM;

    }
    struct addrinfo* get_server_info();

    void receive_client_request(int client_fd);

    void start_server_loop(int sockfd);

    void *get_in_addr(struct sockaddr* curr_addr);
};