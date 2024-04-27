#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

// int MAX_CLIENTS = 5;
// int PORT = 8080;

int main() {
    // // Create a socket for the server
    // int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // if (serverSocket < 0) {
    //     std::cerr << "Error creating socket\n";
    //     return 1;
    // }

    // // Bind the socket to the specified port
    // struct sockaddr_in serverAddr;
    // std::memset(&serverAddr, 0, sizeof(serverAddr));
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    // serverAddr.sin_port = htons(PORT);
    // if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    //     std::cerr << "Error binding socket\n";
    //     close(serverSocket);
    //     return 1;
    // }

    // // Listen for incoming connections
    // if (listen(serverSocket, MAX_CLIENTS) < 0) {
    //     std::cerr << "Error listening on socket\n";
    //     close(serverSocket);
    //     return 1;
    // }

    // std::cout << "Server started. Listening on port " << PORT << std::endl;

    // // Set of file descriptors to monitor for read readiness
    // fd_set readfds;
    // int maxFd = serverSocket + 1;

    // while (true) {
    //     // Clear the set of file descriptors
    //     FD_ZERO(&readfds);
    //     FD_SET(serverSocket, &readfds);

    //     // Add server socket to the set
    //     for (int i = 0; i < MAX_CLIENTS; ++i) {
    //         if (clientSockets[i] > 0) {
    //             FD_SET(clientSockets[i], &readfds);
    //             maxFd = std::max(maxFd, clientSockets[i] + 1);
    //         }
    //     }

    //     // Wait for activity on any of the file descriptors
    //     int activity = select(maxFd, &readfds, NULL, NULL, NULL);
    //     if ((activity < 0) && (errno != EINTR)) {
    //         std::cerr << "Error in select\n";
    //         continue;
    //     }

    //     // Check if the server socket has activity (new connection)
    //     if (FD_ISSET(serverSocket, &readfds)) {
    //         struct sockaddr_in clientAddr;
    //         socklen_t clientLen = sizeof(clientAddr);
    //         int newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    //         if (newSocket < 0) {
    //             std::cerr << "Error accepting connection\n";
    //             continue;
    //         }
    //         std::cout << "New connection accepted\n";

    //         // Add the new socket to the client sockets array
    //         for (int i = 0; i < MAX_CLIENTS; ++i) {
    //             if (clientSockets[i] == 0) {
    //                 clientSockets[i] = newSocket;
    //                 break;
    //             }
    //         }
    //     }

    //     // Check for activity on client sockets
    //     for (int i = 0; i < MAX_CLIENTS; ++i) {
    //         int clientSocket = clientSockets[i];
    //         if (FD_ISSET(clientSocket, &readfds)) {
    //             char buffer[1024] = {0};
    //             ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    //             if (bytesRead <= 0) {
    //                 // Connection closed or error
    //                 close(clientSocket);
    //                 clientSockets[i] = 0;
    //                 std::cout << "Client disconnected\n";
    //             } else {
    //                 // Echo back received data
    //                 send(clientSocket, buffer, bytesRead, 0);
    //             }
    //         }
    //     }
    // }

    // // Close the server socket
    // close(serverSocket);

	int socketServerFd;
	if ((socketServerFd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		std::cerr << "error: server socket creation failed\n";
		return (1);
	}

	int max_clients = 10;
	if (setsockopt(socketServerFd, SOL_SOCKET, SO_REUSEADDR, (char *)&max_clients, sizeof(max_clients)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//binding the socket (naming the secket server)
	int serverPort = 8080;
	sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(serverPort);

	if ((bind(socketServerFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))== -1){
		std::cerr << "error: binding the server socket failed\n";
		close(socketServerFd);
		return (1);
	}
	// // make the serverfd in listen mode
	if (listen(socketServerFd, 10) == - 1){
		std::cerr << "error: socketServer listen failed\n";
		close(socketServerFd);
		return (1);
	}
	int addlen = sizeof(serverAddress);
	std::cout << "server start listening on 8080\n";

	int socketClient[max_clients];
	for (int i = 0; i < max_clients; i++){
		socketClient[i] = 0;
	}

	while(true)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(socketServerFd, &rfds);
		for(int i = 0; i < max_clients; i++){
			if (socketClient[i] > 0)
				FD_SET(socketClient[i], &rfds);
		}
		int activity = select(FD_SETSIZE, &rfds, nullptr, nullptr, nullptr);
		if (activity < 0){
			std::cerr << "select error" << std::endl;
		}
		if (FD_ISSET(socketServerFd, &rfds)) {
			int new_socket = accept(socketServerFd, nullptr, nullptr);
			if (new_socket < 0) {
				std::cerr << "Accept failed\n";
				return EXIT_FAILURE;
			}

			// Add the new socket to the array of client sockets
			for(int i = 0; i < max_clients; i++){
				if (socketClient[i] == 0){
					socketClient[i] = new_socket;
					break;
				}
			}
		}
			// Handle client sockets
		for (int i = 0; i < max_clients; i++) {
			if (FD_ISSET(socketClient[i], &rfds)) {
				char buffer[1024];
				int valread = read(socketClient[i], buffer, 1024);
				if (valread == 0) {
					// Client disconnected
					close(socketClient[i]);
					socketClient[i] = 0;
				} else {
					// Handle data from client
					// For demonstration purposes, we're just printing the received data
					buffer[valread] = '\0';
					std::cout << "Received: " << buffer << std::endl;
					write(socketClient[i], "test", 5);
				}
			}
		}
	}
	// int socketClientFd;
	// if ((socketClientFd = accept(socketServerFd, (struct sockaddr *)&serverAddress, (socklen_t *)&addlen)) < 0){
	// 	std::cerr << "error: in accept\n";
	// 	exit(1);
	// }
	///
	// fd_set rfds;
	// // struct timeval tv;
	// int retval;
	// FD_ZERO(&rfds); /* Watch stdin (fd 0) for input */
	// FD_SET(socketServerFd, &rfds);
	// // tv.tv_sec = 5; /* Wait up to five seconds. */
	// // tv.tv_usec = 0;
	// retval = select(1, &rfds, NULL, NULL, NULL);
	// if (retval == -1)
	// 	perror("select()");
	// else if (retval > 0)
	// 	printf("Data is available now.\n");
	// /* FD_ISSET(0, &rfds) will be true, can use read() */
	// else
	// 	printf("No data within five seconds.\n");

    return 0;
}
