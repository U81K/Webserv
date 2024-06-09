/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 18:56:38 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/24 16:42:51 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ClientData.hpp"

ClientData::ClientData(int fd, struct sockaddr_in a, ServerData &serv) : sockfd(fd), addr(a), server(serv){
	isReqFinished = false;
	// std::cout << req.getContentLen() << std::endl;
	(void) addr;
}

int ClientData::getSocketFd(){
	return (sockfd);
}

bool ClientData::readRequest(char *buffer, size_t bytesReceived){
	std::string buff(buffer, bytesReceived);
	req.addBuffer(buff, bytesReceived);
	if (req.isReqFinished()){
		isReqFinished = true;
		req.parsingRequest();
		return (true);
	}
	// exit(1);
	return (false);
}

bool ClientData::sendResponce(){
	
	if (isReqFinished){
		res.generate(req, server);
		res.sending(sockfd);
		req.printFullReq();
		req.clear();
		res.clear();
		isReqFinished = false;
		// exit(0);
		return (true);
	}
	return (false);
}
