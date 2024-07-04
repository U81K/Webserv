#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

std::map<std::string, std::string> parseHeaders(const std::string &request) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(request);
    std::string line;
    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            headers[line.substr(0, pos)] = line.substr(pos + 2);
        }
    }
    return headers;
}

std::string getCGIResponse(const std::string &scriptPath, std::map<std::string, std::string> env) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        error("pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        error("fork");
    } else if (pid == 0) {
        // Child process
        close(pipefd[0]);  // Close unused read end

        // Set environment variables
        for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++) {
            setenv(it->first.c_str(), it->second.c_str(), 1);
        }

        // Redirect stdout to pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Execute CGI script
        execl(scriptPath.c_str(), scriptPath.c_str(), NULL);
        error("execl");  // If execl fails
    } else {
        // Parent process
        close(pipefd[1]);  // Close unused write end

        // Read CGI script output
        char buffer[BUFFER_SIZE];
        std::string response;
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            response.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        // Wait for child process to finish
        waitpid(pid, NULL, 0);

        return response;
    }
    return "";
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("socket failed");
    }

    // Forcefully attaching socket to the port
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    //     error("setsockopt");
    // }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("bind failed");
    }
    if (listen(server_fd, 3) < 0) {
        error("listen");
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            error("accept");
        }

        read(new_socket, buffer, BUFFER_SIZE);
        std::string request(buffer);
        std::istringstream requestStream(request);
        std::string method, path, version;

        requestStream >> method >> path >> version;

        if (method == "GET" && path == "/cgi-bin/login.cgi") {
            std::map<std::string, std::string> headers = parseHeaders(request);
            std::map<std::string, std::string> env;
            env["GATEWAY_INTERFACE"] = "CGI/1.1";
            env["REQUEST_METHOD"] = "GET";
            env["QUERY_STRING"] = path.substr(path.find('?') + 1);
            env["SCRIPT_FILENAME"] = "/path/to/login.cgi"; // Update with actual path
            env["SCRIPT_NAME"] = "/cgi-bin/login.cgi";
            env["REQUEST_URI"] = path;
            env["SERVER_NAME"] = "localhost";
            env["SERVER_PORT"] = std::to_string(PORT);
            env["REMOTE_PORT"] = std::to_string(ntohs(address.sin_port));

            std::string cgiResponse = getCGIResponse("/path/to/login.cgi", env);

            std::string httpResponse = "HTTP/1.1 200 OK\r\n";
            httpResponse += "Content-Type: text/html\r\n";
            httpResponse += "Content-Length: " + std::to_string(cgiResponse.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += cgiResponse;

            write(new_socket, httpResponse.c_str(), httpResponse.length());
        } else {
            std::string notFoundResponse = "HTTP/1.1 404 Not Found\r\n";
            notFoundResponse += "Content-Length: 0\r\n";
            notFoundResponse += "\r\n";
            write(new_socket, notFoundResponse.c_str(), notFoundResponse.length());
        }

        close(new_socket);
    }

    return 0;
}
