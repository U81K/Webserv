#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>

// Include required headers for sockets and select
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    // Create a master socket
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return EXIT_FAILURE;
    }

	int max_clients = 10;
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&max_clients, sizeof(max_clients)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
    // Prepare the sockaddr_in structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket to localhost:8080
    if (bind(master_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return EXIT_FAILURE;
    }

    // Listen
    if (listen(master_socket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen failed\n";
        return EXIT_FAILURE;
    }

    // Set of socket descriptors
    fd_set readfds;

    // Client sockets array
    std::vector<int> client_sockets(MAX_CLIENTS, 0);

    std::cout << "Server started. Waiting for connections...\n";

    // Accept incoming connections and handle I/O with select
    while (true) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(master_socket, &readfds);
        int max_sd = master_socket;

        // Add client sockets to set
        for (const auto& client_socket : client_sockets) {
            if (client_socket > 0) {
                FD_SET(client_socket, &readfds);
                if (client_socket > max_sd) {
                    max_sd = client_socket;
                }
            }
        }

        // Wait for activity on any of the sockets
        int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity < 0 && errno != EINTR) {
            std::cerr << "Select error\n";
            break;
        }

        // If master socket is set, then there's a new connection
        if (FD_ISSET(master_socket, &readfds)) {
            int new_socket = accept(master_socket, nullptr, nullptr);
            if (new_socket < 0) {
                std::cerr << "Accept failed\n";
                break;
            }

            // Add the new socket to the array of client sockets
            for (auto& client_socket : client_sockets) {
                if (client_socket == 0) {
                    client_socket = new_socket;
                    std::cout << "New connection, socket fd is " << new_socket << std::endl;
                    break;
                }
            }
        }

        // Handle client sockets
        for (auto& client_socket : client_sockets) {
            if (FD_ISSET(client_socket, &readfds)) {
                char buffer[BUFFER_SIZE];
                int valread = read(client_socket, buffer, BUFFER_SIZE);
                if (valread <= 0) {
                    // Error or client disconnected
                    if (valread == 0) {
                        std::cout << "Client disconnected\n";
                    } else {
                        std::cerr << "Read error\n";
                    }
                    close(client_socket);
                    client_socket = 0;
                } else {
                    // Echo back the received data
                    buffer[valread] = '\0';
                    std::cout << "Received: " << buffer;
                    send(client_socket, buffer, valread, 0);
                }
            }
        }
    }

    // Close master socket
    close(master_socket);

    return EXIT_SUCCESS;
}
