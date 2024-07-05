/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 18:47:10 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/23 23:06:53 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ServerData.hpp"
#include <cstdlib>

ServerData::ServerData(std::string servName, std::string ho, std::vector<int> portss, size_t mbz)
	: serverName(servName), host(ho), ports(portss), maxBodySize(mbz) {
	for(unsigned long i = 0; i < ports.size();i++){
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1){
			// perror("socket");
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
			// perror("bind");
			continue;
		}
		if (listen(fd, SOMAXCONN) == -1){
			// perror("listen");
			continue;
		}
		servSockets.push_back(fd);
	}
}

std::vector<int> &ServerData::getServSockets(){
	return (servSockets);
}

void setnonblocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		// perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		// perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}

void ServerData::addLocation(Location loc){
	locations.push_back(loc);
}

std::string ServerData::getServerName() const {
	return (serverName);
}

std::string ServerData::getHost() const {
	return (host);
}

std::vector<Location> ServerData::getLocation() const{
	return (locations);
}

std::vector<int> ServerData::getPorts() const {
	return (ports);
}

bool ServerData::isIaSocket(int i){
	for(unsigned long j = 0; j < servSockets.size(); j++){
		if (i == servSockets[j]){
			return (true);
		}
	}
	return (false);
}

size_t ServerData::getMaxBodySize(){
	return (maxBodySize);
}
