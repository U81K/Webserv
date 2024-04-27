/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:12 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/24 23:42:18 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"

int main(void){
	//creating a socket for the servet
	int socketServerFd;
	if ((socketServerFd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		std::cerr << "error: server socket creation failed\n";
		return (1);
	}
	//binding the socket (naming the secket server)
	int serverPort = 8080;
	sockaddr_in serverAddress;
	int addlen = sizeof(serverAddress);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(serverPort);
	if ((bind(socketServerFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))== -1){
		std::cerr << "error: binding the server socket failed\n";
		close(socketServerFd);
		return (1);
	}
	// make the serverfd in listen mode
	if (listen(socketServerFd, 10) == - 1){
		std::cerr << "error: socketServer listen failed\n";
		close(socketServerFd);
		return (1);
	}
	std::cout << "server start listening on 8080\n";
	// bool stop = false;
	// int buffersize = 1024;
	// char buffer[buffersize];
	
	while (1){
		int socketClientFd;
		if ((socketClientFd = accept(socketServerFd, (struct sockaddr *)&serverAddress, (socklen_t *)&addlen)) < 0){
			std::cerr << "error: in accept\n";
			exit(1);
		}
		write(socketClientFd, "hello world", 11);
		char buffer[3000];
		int readed = recv(socketClientFd, buffer, sizeof(buffer), 0);
		if (readed == -1)
			std::cout << "error receaving data\n";
		else{
			buffer[readed] = '\0';	
			std::cout << buffer << std::endl;
		}
		close(socketClientFd);
	}
	close(socketServerFd);
	return (0);	
}