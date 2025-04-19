#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORTNUM 28723
#define MAX_EVENTS 10
#define BUFFER_SIZE 1500

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    printf(">> Starting unikernel server...\n");
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    setNonBlocking(serverFd);

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORTNUM);

    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", PORTNUM);

    int epollFd = epoll_create1(0);
    if (epollFd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event = {0};
    event.events = EPOLLIN;
    event.data.fd = serverFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event);

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;
            if (fd == serverFd) {
                struct sockaddr_in clientAddr;
                socklen_t clientLen = sizeof(clientAddr);
                int clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientLen);
                if (clientFd >= 0) {
                    setNonBlocking(clientFd);
                    struct epoll_event clientEvent = {0};
                    clientEvent.events = EPOLLIN | EPOLLET;
                    clientEvent.data.fd = clientFd;
                    epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &clientEvent);
                    printf("New client connected: %d\n", clientFd);
                }
            } else {
                char buffer[BUFFER_SIZE];
                int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0) {
                    printf("Client disconnected: %d\n", fd);
                    close(fd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
                } else {
                    buffer[bytesRead] = '\0';
                    printf("Client %d: %s\n", fd, buffer);
                    if (strcmp(buffer, "exit") == 0) {
                        send(fd, buffer, strlen(buffer), 0);
                        close(fd);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
                        printf("Closed client: %d\n", fd);
                        continue;
                    }
                    char reply[BUFFER_SIZE];
                    snprintf(reply, sizeof(reply), "Echo: %s", buffer);
                    send(fd, reply, strlen(reply), 0);
                }
            }
        }
    }

    close(serverFd);
    close(epollFd);
    return 0;
}
