/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 18:47:10 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/04 14:56:23 by khaimer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ServerData.hpp"
#include <cstdlib>

void    ServerData::setServerName(std::string const& Value)
{
    this->serverName = Value;
}

void	ServerData::setHost(std::string const& Value)
{
   this->host = Value;
}

void	ServerData::printport() //solo
{
    for (size_t i = 0; i < this->ports.size(); i++)
    {
        std::cout << this->ports[i] << " ";
    }
    std::cout << "(port)\n";
}

void ServerData::parse_server_ports(const std::string& ports, ServerData& server) 
{
  std::string port_str = ports;

    // std::cout << port_str << "   << HERE\n";
  size_t split = port_str.find(',');
  while (split != std::string::npos) 
  {
    std::string port_number = port_str.substr(0, split);
    int port = std::atoi(port_number.c_str());
    server.ports.push_back(port);
    port_str = port_str.substr(split + 1); // Remove the extracted part from the remaining string
    split = port_str.find(',');
  }
  if (!port_str.empty()) // Handle the last port (if no comma)
  {
    int port = std::atoi(port_str.c_str());
    if (port)
        server.ports.push_back(port);
  }
}

ServerData::ServerData(std::string servName, std::string ho, std::vector<int> portss, size_t mbz)
	: serverName(servName), host(ho), ports(portss), maxBodySize(mbz) {
	for(unsigned long i = 0; i < ports.size();i++){
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

std::vector<int> &ServerData::getServSockets(){
	return (servSockets);
}

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

// void ServerData::addLocation(Location loc){
// 	locations.push_back(loc);
// }

std::string ServerData::getServerName() const {
	return (serverName);
}

std::string ServerData::getHost() const {
	return (host);
}

// std::vector<Location> ServerData::getLocation() const{
// 	return (locations);
// }

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

size_t ServerData::getMaxBodySize()
{
	return (maxBodySize);
}

void	ServerData::setmaxBodySize(std::string const& Value)
{
    this->maxBodySize = std::stoi(Value);
}

void ServerData::setLocation(int number, location locationData)
{
    this->locations[number] = locationData;
}

std::map<int, location> ServerData::get_locations() const
{
    return this->locations;
}
