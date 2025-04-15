// References to server/client implementation in cpp comes from https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/time.h>

// From https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example/blob/master/tcp-Client.cpp
// Simplifies process of using anything in standard library
using namespace std;

#define PORTNUM 28723;
#define MAX_EVENTS 10
#define BUFFER_SIZE 1500

// Basic needs client/server will need
struct
{
    // 0 = Client Mode, 1 = Server Mode
    int executionMode;

    // Generated from https://it-tools.tech/random-port-generator, can be redefined by user
    int portNumber = PORTNUM;
    char msgBuffer[1500];
    char *server_ip = "localhost";
} user;

// One thing to note from example is that communication is handled using IPv4 sockets
// Might try it but will revert to domain sockets if things don't work out
int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void server()
{
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0)
    {
        cerr << "Error creating socket." << endl;
        exit(EXIT_FAILURE);
    }

    // Set server socket as non-blocking
    setNonBlocking(serverSd);

    // Bind socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(user.portNumber);

    if (bind(serverSd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error binding socket." << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSd, SOMAXCONN) < 0)
    {
        cerr << "Error listening on socket." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Listening on port " << user.portNumber << endl;

    // Set up epoll
    int epollFd = epoll_create1(0);
    if (epollFd < 0)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = serverSd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSd, &event);

    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i)
        {
            int eventFd = events[i].data.fd;

            if (eventFd == serverSd)
            {
                // Accept new client
                sockaddr_in clientAddr{};
                socklen_t clientLen = sizeof(clientAddr);
                int clientFd = accept(serverSd, (sockaddr *)&clientAddr, &clientLen);
                if (clientFd >= 0)
                {
                    setNonBlocking(clientFd);
                    epoll_event clientEvent{};
                    clientEvent.events = EPOLLIN | EPOLLET; // edge-triggered
                    clientEvent.data.fd = clientFd;
                    epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &clientEvent);
                    cout << "New client connected: " << clientFd << endl;
                }
            }
            else
            {
                // Handle data from a client
                char buffer[BUFFER_SIZE];
                int bytesRead = recv(eventFd, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0)
                {
                    // Client closed or error
                    cout << "Client disconnected: " << eventFd << endl;
                    close(eventFd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, nullptr);
                }
                else
                {
                    buffer[bytesRead] = '\0';
                    cout << "Client " << eventFd << ": " << buffer << endl;

                    // If message is "exit", close the connection
                    if (strcmp(buffer, "exit") == 0)
                    {
                        send(eventFd, buffer, strlen(buffer), 0);
                        close(eventFd);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, nullptr);
                        cout << "Closed client: " << eventFd << endl;
                        continue;
                    }

                    // Echo back
                    string reply = "Echo: " + string(buffer);
                    send(eventFd, reply.c_str(), reply.size(), 0);
                }
            }
        }
    }

    close(serverSd);
    close(epollFd);
}

void client()
{
    // setup a socket and connection tools
    struct hostent *host = gethostbyname(user.server_ip);
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(user.portNumber);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    // trying to connect
    int status = connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));

    if (status < 0)
    {
        cout << "Error connecting to socket!" << endl;
        return;
    }

    cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    while (1)
    {
        cout << ">";
        string data;
        getline(cin, data);
        memset(&user.msgBuffer, 0, sizeof(user.msgBuffer)); // clear the buffer
        strcpy(user.msgBuffer, data.c_str());
        if (data == "exit")
        {
            send(clientSd, (char *)&user.msgBuffer, strlen(user.msgBuffer), 0);
            break;
        }
        bytesWritten += send(clientSd, (char *)&user.msgBuffer, strlen(user.msgBuffer), 0);
        cout << "Awaiting server response..." << endl;
        memset(&user.msgBuffer, 0, sizeof(user.msgBuffer)); // clear the buffer
        bytesRead += recv(clientSd, (char *)&user.msgBuffer, sizeof(user.msgBuffer), 0);
        if (!strcmp(user.msgBuffer, "exit"))
        {
            cout << "Server has quit the session" << endl;
            break;
        }
        cout << "Server: " << user.msgBuffer << endl;
    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
         << " secs" << endl;
    cout << "Connection closed" << endl;
}

// argc: count of arguments
// argv: array of char* (strings)
int main(int argc, char **argv)
{

    // If 2 arguments (program and server/client) not passed in, throw error
    if (argc != 2)
    {
        cerr << "One argument not passed in" << endl;
        return -1;
    }

    // If second argument is a) not a string or b) not "server" or "client"
    // throw an error and ask for "server" or "client"
    if (strcmp(argv[1], "client") != 0 && strcmp(argv[1], "server") != 0)
    {
        cerr << "Enter 'server' or 'client' to run program" << endl;
        return -1;
    }

    // Initialize params of user depending on passed in values
    if (strcmp(argv[1], "server") == 0)
    {
        user.executionMode = 1;
    }
    else
    {
        user.executionMode = 0;
    }

    // Insert execution functions for server and client here
    // Depending on what was passed in to the command line
    if (user.executionMode)
    {
        cout << "Executing Server" << endl;
        server();
    }
    else
    {
        cout << "Executing Client" << endl;
        client();
    }

    // Defining two types of client server architectures
    // Client: Pass in "client" as an argument
    // Server: Pass in "server" as an argument
    return 0;
}