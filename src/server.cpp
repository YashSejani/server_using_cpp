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
    // 1. Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        cerr << "Socket creation failed.\n";
        return 1;
    }

    // 2. Define server address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 3. Bind socket to address
    if (::bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed.\n";
        return 1;
    }

    // 4. Start listening
    if (listen(server_fd, 5) < 0) {
        cerr << "Listen failed.\n";
        return 1;
    }

    cout << "Server is running at http://localhost:8080\n";

    // 5. Accept client
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        cerr << "Accept failed.\n";
        return 1;
    }

    // 6. Read request
    char buffer[2048] = {0};
    read(client_fd, buffer, sizeof(buffer));
    cout << "Request:\n" << buffer << "\n";

    // 7. Parse request line
    istringstream request_stream(buffer);
    string method, path, http_version;
    request_stream >> method >> path >> http_version;

    string response_body;
    string status_line;

    if (method == "GET") {
        // Convert / to /index.html
        if (path == "/") path = "/index.html";

        // Remove leading /
        string filename = "." + path;

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
    string content_type = get_content_type(path);
    // 8. Build HTTP response
    string response =
        status_line +
        "Content-Type: " + content_type + "\r\n"
        "Content-Length: " + to_string(response_body.length()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + response_body;

    // 9. Send response
    send(client_fd, response.c_str(), response.length(), 0);

    // 10. Clean up
    close(client_fd);
    close(server_fd);

    return 0;
}
