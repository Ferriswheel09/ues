#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORTNUM 28723
#define BUFFER_SIZE 1500

int main() {
    char msgBuffer[BUFFER_SIZE];
    char *server_ip = "127.0.0.1";
    struct hostent *host = gethostbyname(server_ip);

    if (!host) {
        fprintf(stderr, "Failed to resolve hostname\n");
        return -1;
    }

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    serverAddr.sin_port = htons(PORTNUM);

    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd < 0) {
        perror("socket");
        return -1;
    }

    if (connect(clientFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return -1;
    }

    printf("Connected to the server!\n");

    int bytesRead = 0, bytesWritten = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (1) {
        printf(">");
        fflush(stdout);
        char input[BUFFER_SIZE];
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0; // Remove newline

        strcpy(msgBuffer, input);

        if (strcmp(input, "exit") == 0) {
            send(clientFd, msgBuffer, strlen(msgBuffer), 0);
            break;
        }

        bytesWritten += send(clientFd, msgBuffer, strlen(msgBuffer), 0);

        printf("Awaiting server response...\n");

        memset(msgBuffer, 0, sizeof(msgBuffer));
        int n = recv(clientFd, msgBuffer, sizeof(msgBuffer), 0);
        if (n <= 0) break;

        bytesRead += n;

        if (strcmp(msgBuffer, "exit") == 0) {
            printf("Server has quit the session\n");
            break;
        }

        printf("Server: %s\n", msgBuffer);
    }

    gettimeofday(&end, NULL);
    close(clientFd);

    printf("********Session********\n");
    printf("Bytes written: %d Bytes read: %d\n", bytesWritten, bytesRead);
    printf("Elapsed time: %ld secs\n", (end.tv_sec - start.tv_sec));
    printf("Connection closed\n");

    return 0;
}
