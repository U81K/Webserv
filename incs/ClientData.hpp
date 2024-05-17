/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/17 21:14:49 by bgannoun         ###   ########.fr       */
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
#include "../incs/request.hpp"

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		bool isReqFinished;
		request req;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a);
		int getSocketFd();
		bool readRequest(char *buffer, size_t bytesReceived);
		void outputHTTPRequestToFile(const std::string& httpRequest, const std::string& filename);
		bool sendResponce();
};

#endif