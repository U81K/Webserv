/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 18:09:00 by bgannoun          #+#    #+#             */
/*   Updated: 2024/07/05 21:03:27 by khaimer          ###   ########.fr       */
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

class location{
	private:
		std::string path;
		std::map<std::string, std::string> directive;
	public:
		location(const location& other) : path(other.path), directive(other.directive) {}
		location& operator=(const location& other) {
			if (this != &other) {
				path = other.path;
				directive = other.directive;
			}
			return *this;
    	}
		location() {}
		location(const std::string &p) : path(p) {}
		void addDirective(const std::string &direc, const std::string &value){
			directive[direc] = value;
		}
		std::string getDirective(std::string key){
			return (directive[key]);
		}
		std::string getPath() const{
			return (path);
		}
		std::map<std::string, std::string> &getDirectives(){
			return (directive);
		}

		//

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
		std::vector<location> locs;
		size_t maxBodySize;
		std::map<std::string, std::string> errorPages;
	public:
		ServerData(){}
		ServerData( std::string servName,  std::string ho,  std::vector<int> portss, size_t mbz);

		void			setmaxBodySize(std::string const& Value);
		std::string getHost() const;
		std::vector<int> &getServSockets();
		bool isIaSocket(int i);
		void addLocation(location locationData);
		std::string getServerName() const;
		std::vector<location> getLocation() const;
		std::vector<int> getPorts() const;
		size_t		getMaxBodySize() const;

		void			setHost(std::string const& Value);

		void			setServerName(std::string const& Value);
		void				parse_server_ports(const std::string& ports, ServerData& server);

		void start_listen();
		void addLoc(location &lo){
			locs.push_back(lo);
		}
		std::vector<location> getLocs() const{
			return (locs);
		}
		
		void	printport() //solo
		{
			for (size_t i = 0; i < this->ports.size(); i++)
			    std::cout << this->ports[i] << " ";
			std::cout << "(port)\n";
		}
};

#endif