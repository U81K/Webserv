/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/01 17:43:05 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef serverdata_hpp
#define serverdata_hpp

#include <iostream>
#include <vector>
#include "ClientData.hpp"

struct Location{
	std::string path;
	std::string root;
	std::vector<std::string> acceptedMethods;
	std::string index;
	bool autoIndex;
	std::string uploadPath;
	std::string cgiPath;
};

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include  "fcntl.h"

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

class ServerData{
	private:
		std::vector<int> servSockets;
		std::string serverName;
		std::string host;
		std::vector<int> ports;
		std::vector<Location> locations;
		// int sockefd;
		std::vector<ClientData> clients;
	public:
		ServerData(std::string servName, std::string ho, std::vector<int> portss)
			: serverName(servName), host(ho), ports(portss) {
			for(int i = 0; i < ports.size();i++){
				int fd = socket(AF_INET, SOCK_STREAM, 0);
				if (fd == -1){
					perror("socket");
					continue;
				}
				int reuse_addr = 1;
				setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
				setnonblocking(fd);
				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(host.c_str());
				addr.sin_port = htons(ports[i]);
				if ((bind(fd, (struct sockaddr *)&addr, sizeof(addr)))== -1){
					perror("bind");
					continue;
				}
				if (listen(fd, SOMAXCONN) == -1){
					perror("listen");
					continue;
				}
				servSockets.push_back(fd);
			}
		}
		std::vector<int> &getServSockets(){
			return (servSockets);
		}
		bool isIaSocket(int i){
			for(int j = 0; j < servSockets.size(); j++){
				if (i == servSockets[j]){
					return (true);
				}
			}
			return (false);
		}
		void printFds(){
			for(int i = 0; i < servSockets.size(); i++){
				std::cout << servSockets[i] << std::endl;
			}
		}
		void addLocation(Location loc){
			locations.push_back(loc);
		}
		std::string getServerName() const {
			return (serverName);
		}
		std::string getHost() const {
			return (host);
		}
		std::vector<Location> getLocation() const{
			return (locations);
		}
		std::vector<int> getPorts() const {
			return (ports);
		}
		// void setSocketFd(int fd){
		// 	sockefd = fd;
		// }
		// int getSocketFd(){
		// 	return (sockefd);
		// }
		void addClient(const ClientData &clt){
			clients.push_back(clt);
		}
		std::vector<ClientData> &getClients(){
			return (clients);
		}
		// void removeClient(int sockfd) {
		// 	clients.erase(std::remove_if(clients.begin(), clients.end(),
		// 	[sockfd](const ClientData& client) { return client->getSocketFd() == sockfd; }), clients.end());
		// }
};

#endif