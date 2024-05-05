#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() 
{
    // Create the socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    // Identify the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002); // Choose any available port
    server_address.sin_addr.s_addr = INADDR_ANY;
    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // On the server, wait for an incoming connection
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for incoming connections...\n");

    // Accept a connection
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Error accepting connection");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
	printf("Connection accepted from %s:%d\n", client_ip, ntohs(client_address.sin_port));
    // Receive and print messages
    char buffer[3000];
    int bytes_received;
    while (1) {
        // Receive data from the client
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            close(client_socket);
            close(server_socket);
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            buffer[bytes_received] = '\0';
            printf("Received: %s", buffer);
        }
    }

    // Close the socket
    close(client_socket);
    close(server_socket);

    return 0;
}
