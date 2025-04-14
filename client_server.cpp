// References to server/client implementation in cpp comes from https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <sys/time.h>
#include <fstream>

// From https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example/blob/master/tcp-Client.cpp
// Simplifies process of using anything in standard library
using namespace std;

#define PORTNUM 28723;

// Basic needs client/server will need
struct
{
    // 0 = Client Mode, 1 = Server Mode
    int executionMode;

    // Generated from https://it-tools.tech/random-port-generator, can be redefined by user
    int portNumber = PORTNUM;
    char msgBuffer[1500];
} user;

// One thing to note from example is that communication is handled using IPv4 sockets
// Might try it but will revert to domain sockets if things don't work out
void server()
{
    // Setup socket and connection tools
    sockaddr_in serverAddress;
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(user.portNumber);

    // Open socket with internet address
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0)
    {
        cerr << "Error establishing server socket" << endl;
        exit(0);
    }
    int bindStatus = bind(serverSd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }

    cout << "Listening on port " << user.portNumber << endl;
    listen(serverSd, 5);

    // Handle new connections with client via socket descriptor
    sockaddr_in newSockAddress;
    socklen_t newSockAdressSize = sizeof(newSockAddress);

    int newSd = accept(serverSd, (sockaddr *)&newSockAddress, &newSockAdressSize);
    if (newSd < 0)
    {
        cerr << "Error accepting request from client" << endl;
        exit(1);
    }
    cout << "Connected with client!" << endl;

    struct timeval start1, end1;    
    gettimeofday(&start1, NULL);
    // also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    while (1)
    {
        // receive a message from the client (listen)
        cout << "Awaiting client response..." << endl;
        memset(&user.msgBuffer, 0, sizeof(user.msgBuffer)); 
        bytesRead += recv(newSd, (char *)&user.msgBuffer, sizeof(user.msgBuffer), 0);
        if (!strcmp(user.msgBuffer, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        cout << "Client: " << user.msgBuffer << endl;
        cout << ">";
        string data;
        getline(cin, data);
        memset(&user.msgBuffer, 0, sizeof(user.msgBuffer)); // clear the buffer
        strcpy(user.msgBuffer, data.c_str());
        if (data == "exit")
        {
            // send to the client that server has closed the connection
            send(newSd, (char *)&user.msgBuffer, strlen(user.msgBuffer), 0);
            break;
        }
        // send the message to client
        bytesWritten += send(newSd, (char *)&user.msgBuffer, strlen(user.msgBuffer), 0);
    }
    // we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
         << " secs" << endl;
    cout << "Connection closed..." << endl;
    return 0;
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
    }

    // Defining two types of client server architectures
    // Client: Pass in "client" as an argument
    // Server: Pass in "server" as an argument
    cout << "Hello World" << endl;
    return 0;
}