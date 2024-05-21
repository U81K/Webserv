/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 14:33:37 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/19 18:37:27 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/webserv.hpp"

bool isFdOfServers(int fd, std::vector<class ServerData> &servers){
	for(unsigned int i = 0; i < servers.size(); i++){
		std::vector<int> socketsOfOneServ = servers[i].getServSockets();
		for(unsigned int j = 0; j < socketsOfOneServ.size(); j++){
			if (fd == socketsOfOneServ[j])
				return (true);
		}
	}
	return (false);
}

void startServer(std::vector<class ServerData> &servers){
	fd_set currSocketR;
    fd_set readySocketR;
    FD_ZERO(&currSocketR);//add all servers fds to currSocketR
	for(unsigned int i = 0; i < servers.size(); i++){
		std::vector<int> socketsOfOneServ = servers[i].getServSockets();
		for(unsigned int j = 0; j < socketsOfOneServ.size(); j++){
			FD_SET(socketsOfOneServ[j], &currSocketR);
		}
	}
    fd_set currSocketW;
    fd_set readySocketW;
    FD_ZERO(&currSocketW);
	std::map<int, ClientData> clients;
	char buffer[1024 * 400];
	while (true){
		readySocketR = currSocketR;
		readySocketW = currSocketW;
		
		if (select(FD_SETSIZE, &readySocketR, &readySocketW, NULL, NULL) < 0){
			perror("select");
			exit(1);
		}
		for(int i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &readySocketR)){ // i is ready for reading
				if (isFdOfServers(i, servers)){ //there is an incoming connection request from a client waiting to be accepted.
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
					bzero(buffer, 1024 * 400);
					int bytesReceived = 0;
					bytesReceived = recv(i, buffer, sizeof(buffer), 0);
					if (bytesReceived > 0){
						if (clients[i].readRequest(buffer, bytesReceived)){
							std::cout << "client: " << i << " send a request\n";
							FD_CLR(i, &currSocketR);
							FD_SET(i, &currSocketW);
						}
					}
					else if (bytesReceived == 0){
						std::cout << "client: " << i << " disconnected\n";
						clients.erase(i);
						FD_CLR(i, &currSocketR);
						close(i);
					}
					else if (bytesReceived == -1){
						clients.erase(i);
						FD_CLR(i, &currSocketR);
						close(i);
					}
				}
			}
			else if (FD_ISSET(i, &readySocketW)){
				if (clients[i].sendResponce()){
					FD_CLR(i, &currSocketW);
					FD_SET(i, &currSocketR);
				}
			}
		}
	}
}

int main(int ac, char **av){
	if (ac != 2){
		std::cerr << "invalid args\n";
		exit(1);
	}
	(void) av;
	///parsing
	
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
	// std::cout << "server start listening on port 8080\n";
	// // sockets.push_back(serv2.getServSockets());
	startServer(servers);
	// close(servfd);
	return (0);
}