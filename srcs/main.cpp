/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 14:33:37 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/04 14:45:50 by khaimer          ###   ########.fr       */
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

ServerData getServerData(int fd, std::vector<class ServerData> &servers){
	for(int i = 0; i < servers.size(); i++){
		std::vector<int> tmp = servers[i].getServSockets();
		for (int j = 0; j < tmp.size(); j++){
			if (fd == tmp[j])
				return (servers[i]);
		}
	}
	return (ServerData());
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
					//i is the fd of the server
					// we should add this server class to the clientData
					ServerData serv = getServerData(i, servers);
					ClientData tmpData(cltfd, clientAddr, serv);
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

std::vector<ServerData> parseConfigFile(const std::string& filename) {
    std::vector<ServerData> servers;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) // Skip empty lines
            continue;
        if (line.find("[server]") != std::string::npos) {
            int location_number = 1;
            ServerData server; // Start of a new server block
            while (std::getline(file, line)) {
                if (line.empty()) // End of server block
                    continue;
                if (line.find("[server]") != std::string::npos) {
                    // Found a new server block, push the current server and reset
                    servers.push_back(server);
                    server = ServerData();
                    location_number = 1;
                    continue; // Continue to process the new server block
                }
                std::istringstream line_stream(line);
                std::string key, value;
                line_stream >> key >> value;
                if (std::strstr(key.c_str(), "port") != nullptr)
                    server.parse_server_ports(value, server);
                else if (std::strstr(key.c_str(), "serverName") != nullptr)
                    server.setServerName(value);
                else if (std::strstr(key.c_str(), "host") != nullptr)
                    server.setHost(value);
                else if (std::strstr(key.c_str(), "maxBodySize") != nullptr)
				{
						
                    server.setmaxBodySize(value);
				}
                if (std::strstr(line.c_str(), "location:") != nullptr) {
                    int start = line.find_first_of("(") + 1;
                    int end = line.find_first_of(")");
                    location khalil(line.substr(start, end - start));
                    for (size_t i = 0; i < 6; i++) {
                        if (!std::getline(file, line) || line.empty())
                            break;
                        if (std::strstr(line.c_str(), "root") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            khalil.addDirective("root", line.substr(start, line.size() - start - 1));
                        } else if (std::strstr(line.c_str(), "index") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            khalil.addDirective("index", line.substr(start, line.size() - start - 1));
                        } else if (std::strstr(line.c_str(), "cgi_extentions") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            khalil.addDirective("cgi_extentions", line.substr(start, line.size() - start - 1));
                        } else if (std::strstr(line.c_str(), "autoIndex") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            if (std::strstr(line.c_str(), "on") != nullptr)
                                khalil.addDirective("autoIndex", "on");
                            else
                                khalil.addDirective("autoIndex", "off");
                        } else if (std::strstr(line.c_str(), "upload_path") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            khalil.addDirective("upload_path", line.substr(start, line.size() - start - 1));
                        } else if (std::strstr(line.c_str(), "acceptedMethods") != nullptr) {
                            start = line.find_first_of("=") + 1;
                            khalil.addDirective("acceptedMethods", line.substr(start, line.size() - start - 1));
                        }
                    }
                    server.setLocation(location_number, khalil); // Add location to server
                    location_number++;
                }
            }
            servers.push_back(server); // Add the last server block to the vector of servers
        }
    }
    return servers;
}

void printServers(std::vector<ServerData>& servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << std::endl << "[ SERVER " << i << " ]\n";
        servers[i].printport();
        std::cout << servers[i].getServerName() << "  (server name)\n";
        std::cout << servers[i].getHost() << "  (host)\n";
        std::cout << servers[i].getMaxBodySize() << "  (Max Size)\n";

        std::map<int, location> locations = servers[i].get_locations();
        std::cout << "------ Locations ------\n";
        for (std::map<int, location>::iterator it = locations.begin(); it != locations.end(); ++it) {
            std::cout << "[ LOCATION " << it->first << " ]\n";
            it->second.printDirectives();
            std::cout << "-----------------------\n";
        }
        std::cout << "-----------------------\n";
    }
}

int main(int ac, char **av){
	if (ac != 2){
		std::cerr << "invalid args\n";
		exit(1);
	}
	(void) av;
	std::vector<ServerData> servers = parseConfigFile("ourconfig.conf");
    printServers(servers);
	exit(0);
	///parsing
	
	// std::vector<class ServerData> servers;
	// //serv1
	// std::vector<int> ports1;
	// ports1.push_back(8080);
	// ports1.push_back(8081);
	// ServerData serv1("serv1", "127.0.0.1", ports1, 1024);
	// //
	// location lo1("/");
	// lo1.addDirective("return", "/jj/");
	// lo1.addDirective("root", "Sites-available/Server_1");
	// serv1.addLoc(lo1);
	// //
	// // Location loc1;
	// // loc1.path = "/";
	// // serv1.addLocation(loc1);
	// // Location loc2;
	// // loc2.path = "/jj/";
	// // serv1.addLocation(loc2);
	
	// servers.push_back(serv1);
	// //serv2
	// std::vector<int> ports2;
	// ports2.push_back(8082);
	// ports2.push_back(8083);
	// ServerData serv2("serv2", "127.0.0.1", ports2, 0);
	// servers.push_back(serv2);

	// std::cout << "server start listening on port 8080\n";
	// // sockets.push_back(serv2.getServSockets());
	startServer(servers);
	// close(servfd);
	return (0);
}