/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 14:33:37 by bgannoun          #+#    #+#             */
/*   Updated: 2024/07/06 21:52:16 by khaimer          ###   ########.fr       */
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
	for(unsigned int i = 0; i < servers.size(); i++){
		std::vector<int> tmp = servers[i].getServSockets();
		for (unsigned int j = 0; j < tmp.size(); j++){
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
				if (isFdOfServers(i, servers)){ //there is an incoming connection request10000000 from a client waiting to be accepted.
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

bool isNotSpace(int ch)
{
    return !std::isspace(ch);
}

std::string trim(const std::string& str) {
    std::string::const_iterator it = std::find_if(str.begin(), str.end(), isNotSpace);
    std::string::const_reverse_iterator rit = std::find_if(str.rbegin(), str.rend(), isNotSpace);
    if (it == str.end() || rit == str.rend())
        return " ";
    return std::string(it, rit.base());
}

// std::vector<ServerData> parseConfigFile(const std::string& filename) {
//     std::vector<ServerData> servers;
//     std::ifstream file(filename.c_str());
//     std::string line;
//     while (std::getline(file, line)) {
//         if (line.empty()) // Skip empty lines
//             continue;
//         if (line.find("[server]") != std::string::npos) {
//             int location_number = 1;
//             ServerData server; // Start of a new server block
//             while (std::getline(file, line)) {
//                 if (line.empty()) // End of server block
//                     continue;
//                 if (line.find("[server]") != std::string::npos) {// Found a new server block, push the current server and reset 
// 					server.start_listen();
//                     servers.push_back(server);
//                     server = ServerData();
//                     location_number = 1;
//                     continue; // Continue to process the new server block
//                 }
//                 std::istringstream line_stream(line);
//                 std::string key, value;
//                 line_stream >> key >> value;
//                 if (std::strstr(key.c_str(), "port") != NULL)
// 				{
//                 	server.parse_server_ports(value, server);
// 				}
// 				else if (std::strstr(key.c_str(), "serverName") != NULL)
// 				{
					
// 					std::string value2 = value.substr(0, value.find_first_of(";"));
//                     server.setServerName(value2);
	
// 				}
//                 else if (std::strstr(key.c_str(), "host") != NULL)
// 				{
// 					std::string value2 = value.substr(0, value.find_first_of(";"));
//                     server.setHost(value2);

// 				}
//                 else if (std::strstr(key.c_str(), "maxBodySize") != NULL)
// 				{
//                     server.setmaxBodySize(value);
// 				}
//                 if (std::strstr(line.c_str(), "location:") != NULL) {
//                     int start = line.find_first_of("(") + 1;
//                     int end = line.find_first_of(")");
//                     location khalil(line.substr(start, end - start)); //Storing location
//     			for (size_t i = 0; i < 6; i++) {
//                         if (!std::getline(file, line) || line.empty())
//                             break;
//                         if (std::strstr(line.c_str(), "root") != NULL)
// 						{
//                             start = line.find_first_of("=") + 1;
// 							std::string path = trim(line.substr(start, line.size() - start - 1));
// 							if (valid_agrument(path)){
// 								std::cout << "Invalid Path : [" << path << "]" << std::endl;
// 								exit(0);}
//                             khalil.addDirective("root", path);
//                         } 
// 						else if (std::strstr(line.c_str(), "index") != NULL) 
// 						{
//                             start = line.find_first_of("=") + 1;
// 							std::string index = trim(line.substr(start, line.size() - start - 1));
// 							if (valid_agrument(index)){
// 								std::cout << "Invalid Index : [" << index << "]" << std::endl;
// 								exit(0);}
//                             khalil.addDirective("index", index);
//                         } else if (std::strstr(line.c_str(), "cgi_extentions") != NULL) 
// 						{
//                             start = line.find_first_of("=") + 1;
// 							std::string cgi_extentions = trim(line.substr(start, line.size() - start - 1));
// 							if (valid_agrument(cgi_extentions)){
// 								std::cout << "Invalid cgi_extentions : [" << cgi_extentions << "]" << std::endl;
// 								exit(0);}
//                             khalil.addDirective("cgi_extentions", cgi_extentions);
//                         } else if (std::strstr(line.c_str(), "autoIndex") != NULL) {
//                             start = line.find_first_of("=") + 1;
//                             if (std::strstr(line.c_str(), "on") != NULL)
//                                 khalil.addDirective("autoIndex", "on");
//                             else
//                                 khalil.addDirective("autoIndex", "off");
//                         } else if (std::strstr(line.c_str(), "upload_path") != NULL) 
// 						{
//                             start = line.find_first_of("=") + 1;
// 							std::string upload_path = trim(line.substr(start, line.size() - start - 1));
// 							if (valid_agrument(upload_path)){
// 								std::cout << "Invalid upload_path : [" << upload_path << "]" << std::endl;
// 								exit(0);}
//                             khalil.addDirective("upload_path", upload_path);
//                         } else if (std::strstr(line.c_str(), "acceptedMethods") != NULL) 
// 						{
//                             start = line.find_first_of("=") + 1;
// 							std::string acceptedMethods = trim(line.substr(start, line.size() - start - 1));
// 							if (valid_agrument(acceptedMethods)){
// 								std::cout << "Invalid acceptedMethods : [" << acceptedMethods << "]" << std::endl;
// 								exit(0);}
//                             khalil.addDirective("acceptedMethods", acceptedMethods);
//                         }
//                     }
//                     server.addLocation(khalil); // Add location to server
//                     location_number++;
//                 }
//             }
// 			server.start_listen();
//             servers.push_back(server); // Add the last server block to the vector of servers
//         }
//     }
//     return servers;
// }

void parseServerData(const std::string& line, ServerData& server) {
    std::istringstream line_stream(line);
    std::string key, value;
    line_stream >> key >> value;
    if (std::strstr(key.c_str(), "port") != NULL) {
        server.parse_server_ports(value, server);
    } else if (std::strstr(key.c_str(), "serverName") != NULL) {
        std::string value2 = value.substr(0, value.find_first_of(";"));
        server.setServerName(value2);
    } else if (std::strstr(key.c_str(), "host") != NULL) {
        std::string value2 = value.substr(0, value.find_first_of(";"));
        server.setHost(value2);
    } else if (std::strstr(key.c_str(), "maxBodySize") != NULL) {
        server.setmaxBodySize(value);
    }
}

bool	valid_agrument(std::string &line)
{
	bool Result = false;
	
	if (line.find_first_of(" ") != std::string::npos)
		Result = true;
	
	return Result;
}


void parseLocationData(std::ifstream& file, std::string& line, location& loc) {
    int start = line.find_first_of("(") + 1;
    int end = line.find_first_of(")");
    location khalil(line.substr(start, end - start)); // Storing location
    
    for (size_t i = 0; i < 6; i++) 
	{
        if (!std::getline(file, line) || line.empty()) break;
        start = line.find_first_of("=") + 1;
        std::string directive = trim(line.substr(start, line.size() - start - 1));
		if(!line.compare("}"))
			continue;
        if (valid_agrument(directive)) 
		{
			std::cout << "THIS IS THE LINE  {" << line << "}" << std::endl;
            std::cout << "Invalid config file" << std::endl;
            exit(0);
        }
        if (std::strstr(line.c_str(), "root") != NULL)
            loc.addDirective("root", directive);
        else if (std::strstr(line.c_str(), "index") != NULL)
            loc.addDirective("index", directive);
        else if (std::strstr(line.c_str(), "cgi_extensions") != NULL)
            loc.addDirective("cgi_extensions", directive);
        else if (std::strstr(line.c_str(), "autoIndex") != NULL)
            loc.addDirective("autoIndex", directive == "on" ? "on" : "off");
        else if (std::strstr(line.c_str(), "upload_path") != NULL)
            loc.addDirective("upload_path", directive);
        else if (std::strstr(line.c_str(), "acceptedMethods") != NULL)
            loc.addDirective("acceptedMethods", directive);
    }
}

std::vector<ServerData> parseConfigFile(const std::string& filename) {
    std::vector<ServerData> servers;
    std::ifstream file(filename.c_str());
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("[server]") != std::string::npos) {
            int location_number = 1;
            ServerData server;

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                if (line.find("[server]") != std::string::npos) {
                    server.start_listen();
                    servers.push_back(server);
                    server = ServerData();
                    location_number = 1;
                    continue;
                }

                if (line.find("location:") != std::string::npos) {
                    location loc(line.substr(line.find_first_of("(") + 1, line.find_first_of(")") - line.find_first_of("(") - 1));
                    parseLocationData(file, line, loc);
                    server.addLocation(loc);
                    location_number++;
                } else {
                    parseServerData(line, server);
                }
            }
            server.start_listen();
            servers.push_back(server);
        }
    }

    return servers;
}
void printServers(std::vector<ServerData>& servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << std::endl << "[ SERVER " << i << " ]\n";
		std::cout << "\'";
        servers[i].printport();
		std::cout << "\'";

		std::cout << "\'" << servers[i].getServerName() << "\'" << "  (server name)\n";
        std::cout << "\'" << servers[i].getHost() << "\'" << "  (host)\n";
        std::cout << "\'" << servers[i].getMaxBodySize() << "\'" << "  (Max Size)\n";

        std::vector<location> locations = servers[i].getLocation();
        std::cout << "------ Locations ------\n";
        for (std::vector<location>::iterator it = locations.begin(); it != locations.end(); ++it) {
            std::cout << "[ LOCATION " << i << " ]\n";
            it->printDirectives();
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
	///parsing
	
    // 
	// printServers(servers);
	// exit(0);

	//serv1
	// std::vector<int> ports1;
	// ports1.push_back(8080);
	// ports1.push_back(8081);
	// ServerData serv1("serv1", "127.0.0.1", ports1, 10000000);
	
	std::vector<class ServerData> servers;
	servers = parseConfigFile(av[1]);
	printServers(servers);
	startServer(servers);
	// close(servfd);
	return (0);
} 