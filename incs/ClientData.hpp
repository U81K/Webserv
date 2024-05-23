/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/23 17:24:47 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientdata_hpp
#define clientdata_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "request.hpp"
#include "response.hpp"
#include "ServerData.hpp"

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		bool isReqFinished;
		request req;
		response res;
		// ServerData server;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a);
		int getSocketFd();
		bool readRequest(char *buffer, size_t bytesReceived);
		// void outputHTTPRequestToFile(const std::string& httpRequest, const std::string& filename);
		bool sendResponce(std::vector<class ServerData> &servers);
		// void generateResp();
};

#endif