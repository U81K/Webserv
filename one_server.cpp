/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:27:52 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/27 16:39:57 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include "fcntl.h"

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

#include "ClientData.hpp"
#include <map>

bool requestIsFinished(char *buffer, int bytesReceived){
	std::string req = std::string(buffer);
	if (req.find("GET") != std::string::npos){
		if (req.find("\r\n\r\n") != std::string::npos)
			return (true);
	}
	return (false);
}
#include <fstream>
#include <sstream>

void sendIndexHtml(int clientSocket) {
    std::ifstream file("index.html", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open index.html" << std::endl;
        return;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(fileContent.length()) + "\r\n";
    response += "\r\n";
    response += fileContent;

    send(clientSocket, response.c_str(), response.length(), 0);
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
    fd_set currSocketR;
    fd_set readySocketR;
    FD_ZERO(&currSocketR);
    FD_SET(servfd, &currSocketR);
	
    fd_set currSocketW;
    fd_set readySocketW;
    FD_ZERO(&currSocketW);
	std::map<int, ClientData> clients;
	while (true){
		readySocketR = currSocketR;
		readySocketW = currSocketW;
		
		if (select(FD_SETSIZE, &readySocketR, &readySocketW, NULL, NULL) < 0){
			perror("select");
			exit(1);
		}
		for(int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &readySocketR)){ // i is ready for reading
				if (i == servfd){ //there is an incoming connection request from a client waiting to be accepted.
					struct sockaddr_in clientAddr;
					socklen_t clientAddLen = sizeof(clientAddr);
					int cltfd = accept(servfd, (struct sockaddr *)&clientAddr, &clientAddLen);
					std::cout << "client: " << cltfd << " connected\n";
					FD_SET(cltfd, &currSocketR);
					
					ClientData tmpData(cltfd, clientAddr);
					clients[cltfd] = tmpData;
				}
				else{
					///reading the data
					char buffer[1024];
					ssize_t bytesReceived = recv(i, buffer, 1024, 0);
					if (bytesReceived <= 0){
						if (bytesReceived == 0)
							std::cout << "Client disconnected: " << i << std::endl;
						else{
							std::cerr << "Error: Could not receive data from client " << i << std::endl;
						}
						clients.erase(i);
						FD_CLR(i, &currSocketR);
						close(i);
						// FD_CLR(i, &currSocketW);
					}
					else{
						buffer[bytesReceived] = '\0';
						std::cout << "-----------Received data from client " << i << "----------------\n";
						
						std::cout << buffer << std::endl;
						std::cout << "--------------------------------------------------------\n";
						if (requestIsFinished(buffer, bytesReceived)){
							FD_CLR(i, &currSocketR);
							FD_SET(i, &currSocketW);
						}
					}
				}
			}
			else if (FD_ISSET(i, &readySocketW)){
				char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				send(i, res, strlen(res), 0);
				FD_CLR(i, &currSocketW);
				FD_SET(i, &currSocketR);
			}
		}
	}
	close(servfd);
	return (0);
}