/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 21:50:17 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/25 15:04:35 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ServerData.hpp"
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include "fcntl.h"
#include "ClientData.hpp"

void setupServers(std::vector<ServerData> &servers){
	for(std::vector<ServerData>::iterator it = servers.begin(); it != servers.end(); it++){
		//creating a socket for each server
		int socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd == -1){
			std::cerr << "ERROR: socket creation failed\n";
			continue;
		}
		it->setSocketFd(socketFd);
		//binding the socket (Associate the socket with a the address and port)
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(it->getHost().c_str());
		addr.sin_port = htons((it->getPorts())[0]);
		if ((bind(socketFd, (struct sockaddr *)&addr, sizeof(addr)))== -1){
			std::cerr << "ERROR: socket binding failed\n";
			close(it->getSocketFd());
			continue;
		}
		// Set socket to non-blocking mode
		int flag = fcntl(it->getSocketFd(), F_GETFL, 0);
		if (flag == -1){
            std::cerr << "ERROR: failed to get socket flags for server: " << it->getHost() << std::endl;
            close(it->getSocketFd());
            continue;
		}
		if (fcntl(it->getSocketFd(), F_SETFL, flag | O_NONBLOCK) == -1){
			std::cerr << "ERROR: Failed to set socket to non-blocking mode for server: " << it->getHost() << std::endl;
			close(it->getSocketFd());
            continue;
		}
		// make the socket in listen mode
		if (listen(it->getSocketFd(), SOMAXCONN) == -1){
			std::cerr << "ERROR: failed to listen on socket for: " << it->getHost() << std::endl;
			close(it->getSocketFd());
            continue;
		}
		std::cout << it->getHost() << " start listening on " << (it->getPorts())[0] << std::endl;
	}
}

void handleNewConnection(ServerData &serv){
	struct sockaddr_in clientAddr;
	socklen_t clientAddLen = sizeof(clientAddr);
	int clientSocketFd = accept(serv.getSocketFd(), (struct sockaddr *)&clientAddr, &clientAddLen);
	if (clientSocketFd == -1){
		std::cerr << "ERROR: accept\n";
	} else{
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
		int clientPort = ntohs(clientAddr.sin_port);
		
		ClientData clt(clientSocketFd, clientIP, clientPort);
		serv.addClient(clt);
		std::cout << "Accepted connection from " << clientIP << ":" << clientPort << std::endl;
		// char buffer[1024];
		// int readed = recv(clientSocketFd, buffer, sizeof(buffer), 0);
		// if (readed == -1){
		// 	std::cerr << "ERROR: recv\n";
		// } else if (readed == 0){
		// 	std::cout << "connection closed by client\n";
		// }
		// else{
		// 	buffer[readed] = '\0';
		// 	std::cout << buffer << std::endl;
		// 	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
		// 	// write(clientSocketFd, response.c_str(), response.size());
		// }
	}
	// close(clientSocketFd);
}

void handleClientDisconnection(ServerData &serv, int sockfd){
	// serv.removeClient(sockfd);
}

void readingFromClients(ServerData &serv){
	std::cout << "enter readingFromClients\n";
	// for(std::vector<ClientData>::iterator it = serv.getClients().begin() ; it != serv.getClients().end(); it++){
	// 	std::cout << "client fd: " << it->getSocketFd() << std::endl;
	// }
}

void runServers(std::vector<ServerData> &servers){
	fd_set readSet;
    FD_ZERO(&readSet);

    // Find the maximum socket descriptor
    int maxfd = -1;
    for (std::vector<ServerData>::iterator it = servers.begin(); it != servers.end(); it++) {
        int sockfd = it->getSocketFd();
        FD_SET(sockfd, &readSet);
        if (sockfd > maxfd) {
            maxfd = sockfd;
        }
    }
	while (true){
		fd_set tmpReadSet = readSet;
		
		int activity = select(maxfd + 1, &tmpReadSet, NULL, NULL, NULL);
		if (activity == -1){
			std::cerr << "ERROR: select\n";
			break;
		}
		for (std::vector<ServerData>::iterator it = servers.begin(); it != servers.end(); it++){
			int sockfd = it->getSocketFd();
			if (FD_ISSET(sockfd, &tmpReadSet)){
				// Event occurred on the server socket (new connection)
                // Accept the connection and handle it
                // Code to accept new connections...

                // If there's an incoming connection, add it to the read set
                // FD_SET(newsockfd, &readSet);
                // Adjust maxfd if necessary
				handleNewConnection(*it);
				// }
				// FD_SET(sockfd, &tmpReadSet);
				// if (sockfd > maxfd) {
        		// 	maxfd = sockfd;
    			// }
			}
			else{
				// Event occurred on a client socket (data to read)
    			// Handle data from client
    			// Code to read and process data...
				// int readed = recv(clientSocketFd, buffer, sizeof(buffer), 0);
				std::cout << it->getClients().size() << std::endl;
				for(std::vector<ClientData>::iterator itt = it->getClients().begin() ; itt != it->getClients().end(); itt++){
					int clientSocketFd =itt->getSocketFd();
					if (FD_ISSET(clientSocketFd, &readSet)){
						readingFromClients(*it);
					}
					// std::cout << "client fd: " << itt->getSocketFd() << std::endl;
				}
				// it->getClients().end();
				// for(std::vector<ClientData>::iterator itt = it->getClients().begin(); it != it->getClients().end(); it++){
					
				// }
			}
		}
	}
}

int main(void){
	std::vector<ServerData> servers;
	std::vector<int> serv1_port;
	serv1_port.push_back(8080);
	ServerData serv1("serv1", "127.0.0.1", serv1_port);
	servers.push_back(serv1);
	setupServers(servers);
	runServers(servers);
	return (0);
}