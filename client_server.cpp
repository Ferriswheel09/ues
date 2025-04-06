// References to server/client implementation in cpp comes from https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example
#include <iostream>
#include <string.h>


// From https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example/blob/master/tcp-Client.cpp
// Simplifies process of using anything in standard library
using namespace std;

#define PORTNUM 28723;

// Basic needs client/server will need
struct {
    // 0 = Client Mode, 1 = Server Mode
    int executionMode; 

    // Generated from https://it-tools.tech/random-port-generator, can be redefined by user
    int portNumber = PORTNUM;
    char msgBuffer[1500];
} user;

// argc: count of arguments
// argv: array of char* (strings)
int main(int argc, char **argv) {

    // If 2 arguments (program and server/client) not passed in, throw error
    if(argc != 2){
        cerr << "One argument not passed in" << endl; 
        return -1;
    }

    // If second argument is a) not a string or b) not "server" or "client"
    // throw an error and ask for "server" or "client"
    if(strcmp(argv[1], "client") != 0 && strcmp(argv[1], "server") != 0){
        cerr << "Enter 'server' or 'client' to run program" << endl;
        return -1;
    }
    
    // Initialize params of user depending on passed in values
    if(strcmp(argv[1], "server") == 0){
        user.executionMode = 1; 
    } else {
        user.executionMode = 0;
    }

    // Insert execution functions for server and client here
    // Depending on what was passed in to the command line
    if(user.executionMode){
        cout << "Executing Server" << endl;
    } else {
        cout << "Executing Client" << endl;
    }
    
    // Defining two types of client server architectures
    // Client: Pass in "client" as an argument
    // Server: Pass in "server" as an argument
    cout << "Hello World" << endl; 
    return 0;
}