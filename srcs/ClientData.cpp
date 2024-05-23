/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 18:56:38 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/23 17:28:19 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ClientData.hpp"

ClientData::ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
	isReqFinished = false;
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
	return (false);
}

// void ClientData::outputHTTPRequestToFile(const std::string& httpRequest, const std::string& filename) {
// 	// Open the output file
// 	std::ofstream outFile(filename);
// 	if (!outFile.is_open()) {
// 		std::cerr << "Failed to open file: " << filename << std::endl;
// 		return;
// 	}
// 	// Write the HTTP request string to the file
// 	outFile << httpRequest;
// 	outFile.close();
// }

bool ClientData::sendResponce(std::vector<class ServerData> &servers){
	if (isReqFinished){
		res.generate(req);
		res.sending(sockfd);
		req.printFullReq();
		req.clear();
		isReqFinished = false;
		return (true);
	}
	return (false);
}
