/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/04 14:56:12 by khaimer          ###   ########.fr       */
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
		location(){}
		location(const std::string &p) : path(p) {}
		void addDirective(const std::string &direc, const std::string &value){
			directive[direc] = value;
		}
		std::string getDirective(const std::string &key)
		{
			std::map<std::string, std::string>::iterator it = directive.find(key);
			if (it != directive.end())
				return (it->second);
			return ("");
		}
		void printDirectives(){
        for (std::map<std::string, std::string>::const_iterator it = directive.begin(); it != directive.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    }
};
class ServerData{
	private:
		std::vector<int> servSockets;
		std::string serverName;
		std::string host;
		std::vector<int> ports;
		std::map<int, location> locations;
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
		void	setServerName(std::string const& Value);
		void	setHost(std::string const& Value);
		void	setmaxBodySize(std::string const& Value);
		void	parse_server_ports(const std::string& ports, ServerData& server);
		std::map<int, location> get_locations() const;
		void	printport();
		void	setLocation(int number, location locationData);
		size_t getMaxBodySize();
		// void addLoc(location &lo){ //NOT USED ANYMORE
		// 	 locs.push_back(lo);
		// }
		// std::vector<location> getLocs() const{
		// 	return (locs);
		// }
};

#endif