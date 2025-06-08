#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main() {
    // 1. Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket creation failed\n";
        return 1;
    }

    // 2. Configure server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4221); // Port 4221

    // 3. Bind socket to port
    if (::bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Bind failed\n";
        return 1;
    }

    // 4. Start listening
    if (listen(server_fd, 5) < 0) {
        cerr << "Listen failed\n";
        return 1;
    }

    cout << "Server listening on port 4221...\n";

    // 5. Handle connections
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            cerr << "Accept failed\n";
            continue;
        }

        // 6. Send HTTP response
        const char* response = "HTTP/1.1 200 OK\r\n\r\n";
        send(client_fd, response, strlen(response), 0);
        
        close(client_fd);
    }

    close(server_fd);
    return 0;
}