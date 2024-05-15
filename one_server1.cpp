/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_server1.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 11:00:15 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/27 14:20:17 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include "fcntl.h"
#include "ClientData.hpp"
#include <map>

void setnonblocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}


void sendResponse(int clientSocket) {
    std::string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 10\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>\r\n";

    // Send the HTTP response to the client
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    if (bytesSent == -1) {
        std::cerr << "Error: Could not send HTTP response to client\n";
    } else if (static_cast<size_t>(bytesSent) != response.length()) {
        std::cerr << "Error: Incomplete HTTP response sent to client\n";
    } else {
        std::cout << "HTTP response sent successfully to client\n";
    }
}



int main(void){
	//setup of the server socket
	int servfd = socket(AF_INET, SOCK_STREAM, 0);
	if (servfd == -1){
		perror("socket");
		exit (1);
	}
	int reuse_addr = 1;
	/* So that we can re-bind to it without TIME_WAIT problems */
	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	/* Set socket to non-blocking with our setnonblocking routine */
	setnonblocking(servfd);
	//binding the socket (Associate the socket with a the address and port)
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	if ((bind(servfd, (struct sockaddr *)&addr, sizeof(addr)))== -1){
		perror("bind");
		close(servfd);
		return (1);
	}
	// make the socket in listen mode
	if (listen(servfd, SOMAXCONN) == -1){
		perror("listen");
		close(servfd);
		exit (1);
	}
	std::cout << "server start listening on port 8080\n";
	// Set of file descriptors to monitor
	fd_set readFds;
	fd_set writeFds;
	std::map<int, ClientData> clients;
	int cltFd;
	while (true){
		FD_ZERO(&readFds);
		FD_ZERO(&writeFds);

		FD_SET(servfd, &readFds);
		// for(std::map<int, ClientData>::iterator it = clients.begin(); it != clients.end(); it++){
		// 	int cltSocket = it->first;
		// 	FD_SET(cltSocket, &readFds);
		// 	std::cout << "client: " << cltSocket << " added to readFds" << std::endl;
		// 	// FD_SET(cltSocket, &writeFds);
		// }
		if (select(FD_SETSIZE, &readFds, &writeFds, NULL, NULL) < 0){
			perror("select");
			exit(1);
		}
		for(int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &readFds)){
				if (i == servfd){
					struct sockaddr_in clientAddr;
					socklen_t clientAddLen = sizeof(clientAddr);
					int cltfd = accept(servfd, (struct sockaddr *)&clientAddr, &clientAddLen);
					FD_SET(cltfd, &readFds);
					// ClientData tmpData(cltfd, clientAddr);
					// clients[cltfd] = tmpData;
					if (cltfd == -1){
						std::cerr << "Error: Could not accept incoming connection\n";
						continue;
					}
					// std::cout << "client " << clients[cltfd].getSocketFd() << " connected" << std::endl;
				}
				else{
					char buffer[1024];
					ssize_t bytesReceived = recv(i, buffer, 1024, 0);
					if (bytesReceived <= 0){
						if (bytesReceived == 0)
							std::cout << "Client disconnected: " << i << std::endl;
						else{
							std::cerr << "Error: Could not receive data from client " << i << std::endl;
						}
						// clients.erase(i);
						FD_CLR(i, &readFds);
						close(i);
						// FD_CLR(i, &currSocketW);
					}
					else{
						buffer[bytesReceived] = '\0';
						std::cout << "-----------Received data from client " << i << "----------------\n";
						
						std::cout << buffer << std::endl;
						std::cout << "--------------------------------------------------------\n";
						const char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
						FD_CLR(i, &readFds);
						send(i, res, strlen(res), 0);
					}
				}
			}
		}
		// for(std::map<int, ClientData>::iterator it = clients.begin(); it != clients.end(); it++){
		// 	int cltSocket = it->second.getSocketFd();
		// 	std::cout << "client add to map: " << cltSocket << std::endl;
		// 	FD_SET(cltSocket, &writeFds);
		// }
		// if (select(FD_SETSIZE, NULL, &writeFds, NULL, NULL) < 0) {
        //     perror("select");
        //     exit(1);
        // }

        // for (std::map<int, ClientData>::iterator it = clients.begin(); it != clients.end(); it++) {
        //     if (FD_ISSET(it->second.getSocketFd(), &writeFds)) {
        //         const char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        //         send(it->first, res, strlen(res), 0);
        //     }
        // }
	}
	close(servfd);
	return (0);
}