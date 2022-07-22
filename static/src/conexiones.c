#include "conexiones.h"

int iniciar_servidor(char* ip, char* port) {
    int optVal = 1;
    struct addrinfo hints;
    struct addrinfo* serverInfo;

    memset(&hints, 0, sizeof(hints));   // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // fill in my IP for me

    int rv = getaddrinfo(ip, port, &hints, &serverInfo);
    if (rv != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(rv));
        return EXIT_FAILURE;
    }

    int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (serverSocket == -1) {
        printf("Socket creation error\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(int)) < 0){
        error("setsockopt(SO_REUSEPORT) failed%s\n",strerror(errno));
        return EXIT_FAILURE;
    }

    int sockOpt = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    if (sockOpt == -1) {
        printf("Sockopt error (SO_REUSEADDR)\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int bindVal = bind(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (bindVal == -1) {
        printf("Mismatched bind\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int listenVal = listen(serverSocket, SOMAXCONN);
    if (listenVal == -1) {
        printf("Listen error\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    freeaddrinfo(serverInfo);

    return serverSocket;
}

int conectar_a_servidor(char* ip, char* port) {
    int conn;
    struct addrinfo hints;
    struct addrinfo* serverInfo;
    struct addrinfo* p;

    memset(&hints, 0, sizeof(hints));   // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;        // fill in my IP for me

    int rv = getaddrinfo(ip, port, &hints, &serverInfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(rv));
        return EXIT_FAILURE;
    }
    for(p = serverInfo; p != NULL; p = p->ai_next) {
        conn = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(conn == -1) {
            continue;
        }
        if(connect(conn, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(conn);
    }
    freeaddrinfo(serverInfo);
    if(conn != -1 && p != NULL) {
        return conn;
    }
    return -1;
}
