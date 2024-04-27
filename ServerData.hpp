/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/25 14:51:24 by bgannoun         ###   ########.fr       */
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

class ServerData{
	private:
		std::string serverName;
		std::string host;
		std::vector<int> ports;
		std::vector<Location> locations;
		int sockefd;
		std::vector<ClientData> clients;
	public:
		ServerData(std::string servName, std::string ho, std::vector<int> portss)
			: serverName(servName), host(ho), ports(portss) {}
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
		void setSocketFd(int fd){
			sockefd = fd;
		}
		int getSocketFd(){
			return (sockefd);
		}
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