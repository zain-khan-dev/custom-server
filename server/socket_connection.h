struct SocketConnection {
    struct addrinfo curr_host;
    int sockfd;

    SocketConnection() {
        memset(&curr_host, 0, sizeof(curr_host));
        curr_host.ai_flags = AF_UNSPEC;
        curr_host.ai_socktype = SOCK_STREAM;
    }

    struct addrinfo get_curr_host();
    int create_socket_connection(struct addrinfo*);
    void listen_for_connections();
    struct addrinfo* get_server_info(struct addrinfo);
};