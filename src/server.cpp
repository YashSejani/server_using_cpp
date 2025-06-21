#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>

using namespace std;

string read_file(const string& filename) {
    ifstream file(filename);
    if (!file) return "";

    ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
bool ends_with(const string& value, const string& suffix) {
    if (suffix.size() > value.size()) return false;
    return equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

string get_content_type(const string& path) {
    if (ends_with(path, ".html")) return "text/html";
    if (ends_with(path, ".css")) return "text/css";
    if (ends_with(path, ".js")) return "application/javascript";
    if (ends_with(path, ".png")) return "image/png";
    if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) return "image/jpeg";
    if (ends_with(path, ".gif")) return "image/gif";
    return "text/plain";
}

int main() {
    // 1. Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket creation failed\n";
        return 1;
    }

    // 2. Define server address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

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

    cout << "Server is running at http://localhost:8080\n";

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
