#include <iostream>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using namespace std::chrono;

#define SERVER_IP "127.0.0.1" // change if needed
#define SERVER_PORT 28723
#define NUM_MESSAGES 1000
#define MESSAGE "benchmark"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket creation error" << endl;
        return -1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        return -1;
    }

    char buffer[1500] = {0};
    auto start = high_resolution_clock::now();

    for (int i = 0; i < NUM_MESSAGES; ++i) {
        send(sock, MESSAGE, strlen(MESSAGE), 0);
        recv(sock, buffer, sizeof(buffer), 0);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Sent " << NUM_MESSAGES << " messages." << endl;
    cout << "Total time: " << duration.count() << " ms" << endl;
    cout << "Average RTT per message: " << (double)duration.count() / NUM_MESSAGES << " ms" << endl;

    close(sock);
    return 0;
}
