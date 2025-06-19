#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream> 

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main() {
    // 1. Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    // 2. Define server address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 3. Bind socket to address
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed.\n";
        return 1;
    }

    // 4. Start listening
    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed.\n";
        return 1;
    }

    std::cout << "Server is running at http://localhost:8080\n";

    // 5. Accept client
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "Accept failed.\n";
        return 1;
    }

    // 6. Read request
    char buffer[2048] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "Request:\n" << buffer << "\n";

    // 7. Parse request line
    std::istringstream request_stream(buffer);
    std::string method, path, http_version;
    request_stream >> method >> path >> http_version;

    std::string response_body;
    std::string status_line;

    if (method == "GET") {
        // Convert / to /index.html
        if (path == "/") path = "/index.html";

        // Remove leading /
        std::string filename = "." + path;

        response_body = read_file(filename);

        if (response_body.empty()) {
            status_line = "HTTP/1.1 404 Not Found\r\n";
            response_body = "<h1>404 Not Found</h1>";
        } else {
            status_line = "HTTP/1.1 200 OK\r\n";
        }
    } else {
        status_line = "HTTP/1.1 405 Method Not Allowed\r\n";
        response_body = "<h1>405 Method Not Allowed</h1>";
    }

    // 8. Build HTTP response
    std::string response =
        status_line +
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + response_body;

    // 9. Send response
    send(client_fd, response.c_str(), response.length(), 0);

    // 10. Clean up
    close(client_fd);
    close(server_fd);

    return 0;
}
