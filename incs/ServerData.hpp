/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/28 12:46:01 by bgannoun         ###   ########.fr       */
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
#include <map>

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

class location{
	private:
		std::string path;
		std::map<std::string, std::string> directive;
	public:
		location(const std::string &p) : path(p) {}
		void addDirective(const std::string &direc, const std::string &value){
			directive[direc] = value;
		}
		std::string getDirective(const std::string &key){
			std::map<std::string, std::string>::iterator it = directive.find(key);
			if (it != directive.end())
				return (it->second);
			return ("");
		}
};

class ServerData{
	private:
		std::vector<int> servSockets;
		std::string serverName;
		std::string host;
		std::vector<int> ports;
		std::vector<Location> locations;
		std::vector<location> locs;
		size_t maxBodySize;
		std::map<std::string, std::string> errorPages;
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
		void addLoc(location &lo){
			locs.push_back(lo);
		}
		std::vector<location> getLocs() const{
			return (locs);
		}
};

#endif