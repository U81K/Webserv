/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   one_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:27:52 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/29 23:21:51 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "unistd.h"
#include "fcntl.h"
#include "string.h"

// void setnonblocking(int sock)
// {
// 	int opts;

// 	opts = fcntl(sock,F_GETFL);
// 	if (opts < 0) {
// 		perror("fcntl(F_GETFL)");
// 		exit(EXIT_FAILURE);
// 	}
// 	opts = (opts | O_NONBLOCK);
// 	if (fcntl(sock,F_SETFL,opts) < 0) {
// 		perror("fcntl(F_SETFL)");
// 		exit(EXIT_FAILURE);
// 	}
// 	return;
// }

#include "ClientData.hpp"
#include <map>


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

#include "ServerData.hpp"
#include <algorithm>

class config{
	private:
		std::string config_path;
	public:
		config(std::string path);

};

config::config(std::string config_path)
{
	this->config_path = config_path;
}

int main(int ac , char **av)
{
	std::string c_path = av[1];
	config conf(c_path);// khas l config file ytparssa  
	std::vector<class ServerData> servers;
	//serv1
	std::vector<int> ports1;
	ports1.push_back(8080);
	ports1.push_back(8081);
	ServerData serv1("serv1", "127.0.0.1", ports1);
	servers.push_back(serv1);
	//serv2
	std::vector<int> ports2;
	ports2.push_back(8082);
	ports2.push_back(8083);
	ServerData serv2("serv2", "127.0.0.1", ports2);
	servers.push_back(serv2);
	std::cout << "server start listening on port 8080\n";
	std::vector<int> sockets;
	for(int i = 0; i < servers.size(); i++){
		std::vector<int> servFds = servers[i].getServSockets();
		for (int j = 0; j < servFds.size(); j++){
			sockets.push_back(servFds[j]);
		}
	}
	// sockets.push_back(serv2.getServSockets());
    fd_set currSocketR;
    fd_set readySocketR;
    FD_ZERO(&currSocketR);//add all servers fds to currSocketR
	for(int i = 0; i < sockets.size(); i++){
		FD_SET(sockets[i], &currSocketR);
	}
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
				if (std::find(sockets.begin(), sockets.end(), i) != sockets.end()){ //there is an incoming connection request from a client waiting to be accepted.
					struct sockaddr_in clientAddr;
					socklen_t clientAddLen = sizeof(clientAddr);
					int cltfd = accept(i, (struct sockaddr *)&clientAddr, &clientAddLen);
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
					}
					else{
						buffer[bytesReceived] = '\0';
						if (clients[i].readRequest(buffer, bytesReceived)){
							FD_CLR(i, &currSocketR);
							FD_SET(i, &currSocketW);
						}
						
						// if (requestIsFinished(buffer, bytesReceived)){
						// 	clients[i].finishReq(buffer);
						// 	FD_CLR(i, &currSocketR);
						// 	FD_SET(i, &currSocketW);
						// }
						// else
						// 	clients[i].appendReq(buffer);
					}
				}
			}
			else if (FD_ISSET(i, &readySocketW)){
				clients[i].sendResponce();
				FD_CLR(i, &currSocketW);
				FD_SET(i, &currSocketR);
			}
		}
	}
	// close(servfd);
	return (0);
}