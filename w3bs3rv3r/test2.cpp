#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>

#define PORT 8080
#define BUFFER_SIZE 1024

void handleClient(int clientSocket);
void sendResponse(int clientSocket, const std::string& status, const std::string& contentType, const std::string& content);
void handleGetRequest(int clientSocket, const std::string& path);
void handlePostRequest(int clientSocket, const std::string& path, const std::string& data);
void handleDeleteRequest(int clientSocket, const std::string& path);
std::string listDirectory(const std::string& path);
std::string getMimeType(const std::string& path);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }
        handleClient(clientSocket);
        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE);
    if (bytesRead < 0) {
        std::cerr << "Failed to read from socket." << std::endl;
        return;
    }

    std::istringstream request(buffer);
    std::string method, path, version;
    request >> method >> path >> version;

    std::cout << "Method: " << method << ", Path: " << path << ", Version: " << version << std::endl;

    if (method == "GET") {
        handleGetRequest(clientSocket, path);
    } else if (method == "POST") {
        std::string data;
        std::getline(request, data);
        handlePostRequest(clientSocket, path, data);
    } else if (method == "DELETE") {
        handleDeleteRequest(clientSocket, path);
    } else {
        sendResponse(clientSocket, "400 Bad Request", "text/plain", "Unsupported method.");
    }
}

void sendResponse(int clientSocket, const std::string& status, const std::string& contentType, const std::string& content) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "\r\n";
    response << content;

    write(clientSocket, response.str().c_str(), response.str().size());
}

void handleGetRequest(int clientSocket, const std::string& path) {
    std::string actualPath = "." + path;
    struct stat pathStat;
    stat(actualPath.c_str(), &pathStat);

    if (S_ISDIR(pathStat.st_mode)) {
        sendResponse(clientSocket, "200 OK", "text/html", listDirectory(actualPath));
    } else {
        std::ifstream file(actualPath.c_str(), std::ios::in | std::ios::binary);
        if (!file) {
            sendResponse(clientSocket, "404 Not Found", "text/plain", "File not found.");
            return;
        }

        std::ostringstream fileContent;
        fileContent << file.rdbuf();
        sendResponse(clientSocket, "200 OK", getMimeType(actualPath), fileContent.str());
    }
}

void handlePostRequest(int clientSocket, const std::string& path, const std::string& data) {
    std::string actualPath = "." + path;
    std::ofstream file(actualPath.c_str(), std::ios::out | std::ios::binary);
    if (!file) {
        sendResponse(clientSocket, "500 Internal Server Error", "text/plain", "Failed to write file.");
        return;
    }
    file << data;
    sendResponse(clientSocket, "200 OK", "text/plain", "File created/updated.");
}

void handleDeleteRequest(int clientSocket, const std::string& path) {
    std::string actualPath = "." + path;
    std::cout << "Delete path: " << actualPath << std::endl;
    struct stat pathStat;
    if (stat(actualPath.c_str(), &pathStat) != 0) {
        sendResponse(clientSocket, "404 Not Found", "text/plain", "File or directory not found.");
        return;
    }

    if (S_ISDIR(pathStat.st_mode)) {
        if (rmdir(actualPath.c_str()) != 0) {
            sendResponse(clientSocket, "500 Internal Server Error", "text/plain", "Failed to delete directory.");
            return;
        }
    } else {
        if (remove(actualPath.c_str()) != 0) {
            sendResponse(clientSocket, "500 Internal Server Error", "text/plain", "Failed to delete file.");
            return;
        }
    }
    sendResponse(clientSocket, "200 OK", "text/plain", "File or directory deleted.");
}
#include<string.h>
std::string listDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return "Failed to open directory.";
    }

    std::ostringstream directoryContent;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s", directoryContent.str().c_str());
        directoryContent << entry->d_name << "\r\n";
    }
    closedir(dir);
    return directoryContent.str();
}


std::string getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".gif") != std::string::npos) return "image/gif";
    return "application/octet-stream";
}

