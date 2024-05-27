/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/23 23:06:26 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef serverdata_hpp
#define serverdata_hpp

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include  "fcntl.h"

void setnonblocking(int sock);

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
		std::vector<int> servSockets;
		std::string serverName;
		std::string host;
		std::vector<int> ports;
		std::vector<Location> locations;
		size_t maxBodySize;
	public:
		ServerData(){}
		ServerData(std::string servName, std::string ho, std::vector<int> portss, size_t mbz);
		std::vector<int> &getServSockets();
		bool isIaSocket(int i);
		void addLocation(Location loc);
		std::string getServerName() const;
		std::string getHost() const;
		std::vector<Location> getLocation() const;
		std::vector<int> getPorts() const;
		size_t getMaxBodySize();
};

#endif