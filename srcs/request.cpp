/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 19:05:03 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/13 18:58:27 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/request.hpp"
#include <stdlib.h>

request::request(){
	method = UNKNOWN;
	addBufCouter = 0;
	isChunked = false;
	boundaryFound = false;
	contentLend = -1;
	bytesReaded = 0;
}

void request::addBuffer(std::string &buff, size_t bytesRec){
	///get the method and content counte
	bytesReaded += bytesRec;
	if (addBufCouter == 0){
		size_t firstLinePos = buff.find("\n");
		if (firstLinePos != std::string::npos){
			std::string methodLine = buff.substr(0, firstLinePos);
			if (methodLine.find("GET") != std::string::npos)
				method = GET;
			else if (buff.find("POST") != std::string::npos)
				method = POST;
			else if (buff.find("DELETE") != std::string::npos)
				method = DELETE;
			else
				method = UNKNOWN;
		}
		//get headers
		size_t rnPos = buff.find("\r\n\r\n");
		if (rnPos != std::string::npos){
			headersString = buff.substr(0, rnPos);
		}
		// std::cout << headersString << std::endl;
		//check if the data is chunked
		if (headersString.find("Transfer-Encoding: chunked") != std::string::npos)
			isChunked = true;
		//check if theres a boudary
		size_t bouPos = headersString.find("boundary=");
		if (bouPos != std::string::npos){
			boundaryFound = true;
			bouPos += 9;
			// boundary = req.substr(bouPos);
			size_t newLine = headersString.find("\n", bouPos);
			if (newLine != std::string::npos){
				boundary = headersString.substr(bouPos, newLine - bouPos - 1);
				boundary.append("--");
			}
		}
		//checking Content-Length:
		size_t clPos = headersString.find("Content-Length: ");
		if (clPos != std::string::npos){
			clPos += 16;
			contentLend = strtod((headersString.substr(clPos)).c_str(), NULL);
		}
		addBufCouter++;
	}
	fullReq.push_back(buff);
}

bool request::isReqFinished(){
	if (method == GET || method == DELETE)
		return (true);
	else if (isChunked){
		for (unsigned long i = 0; i < fullReq.size(); i++){
			if (fullReq[i].find("0\r\n\r\n") != std::string::npos)
				return (true);
		}
	}
	else if (contentLend > 0){
		if (contentLend == std::string::npos)
			return (true);
		if (boundaryFound){
			for (unsigned long i = 0; i < fullReq.size(); i++){
				if (fullReq[i].find(boundary) != std::string::npos){
					// std::cout << boundary << std::endl;
					return (true);
				}
			}
		}
		else if(contentLend == (bytesReaded - 4 - headersString.size()))///there is no boundary
			return (true);
	}
	else if (contentLend == 0){
		return (true);
	}
	return (false);
}

void request::printFullReq(){
	for(unsigned long i = 0; i < fullReq.size(); i++){
		std::cout << fullReq[i];
	}
	std::cout << std::endl;
}

void request::clear() {
	fullReq.clear();
	method = UNKNOWN;
	addBufCouter = 0;
	headersString.clear();
	isChunked = false;
	boundary.clear();
	boundaryFound = false;
	contentLend = -1;
	bytesReaded = 0;
}

void request::parsingRequest(){
	headersMap.clear();
	for(unsigned int i = 0; i < headersString.size(); i++){
		if (headersString.at(i) == '\r'){
			headersString.at(i) = ' ';
		}
	}
	std::istringstream iss(headersString);
	std::string line;
	int i = 0;
	while(std::getline(iss, line)){
		if (i == 0){
			std::istringstream stLine(line);
			std::string part;
			std::vector<std::string> vecParts;
			while (stLine >> part){
				vecParts.push_back(part);
			}
			url = vecParts[1];
			httpVersion = vecParts[2];
		}else{
			/// fill the map of the headers
			size_t splitPos = line.find(": ");
			if (splitPos != std::string::npos){
				std::string key = line.substr(0, splitPos);
				std::string value = line.substr(splitPos+2, line.size() - 1);
				headersMap[key] = value;
			}
		}
		i++;
	}
	///getting the body
	bool deleFound = false;
	for (unsigned int i = 0; i < fullReq.size(); i++){
		if (fullReq[i].find("\r\n\r\n") != std::string::npos && !deleFound){
			// std::cout << fullReq[i].find("\r\n\r\n") << std::endl;
			bodyString = fullReq[i].substr(fullReq[i].find("\r\n\r\n")+4);
			deleFound = true;
		}
		else
			bodyString.append(fullReq[i]);
	}
	/// the body includes the boundary
	// std::cout << bodyString << std::endl;
	// std::string boundary = bodyString.substr(0, bodyString.find("\r\n"));
	// size_t contentStart = bodyString.find("\r\n\r\n") + 4;
	// bodyString = bodyString.substr(contentStart);
	// if (bodyString.find(boundary) != std::string::npos){
	// 	bodyString = bodyString.substr(0, bodyString.find(boundary));
	// }
	// std::cout << bodyString;
	// std::ofstream file("image.jpg", std::ios::binary);
	// if (!file.is_open()) {
    //     std::cerr << "Failed to open file: image.jpg" << std::endl;
    //     return;
    // }
	// file.write(bodyString.data(), bodyString.size());
	// file.close();
}

request::Method request::getMethod(){
	return (method);
}

std::string request::getUrl(){
	return (url);
}

std::string request::getHttpV(){
	return(httpVersion);
}

std::map<std::string, std::string> request::getHeadersMap(){
	return (headersMap);
}

std::string request::getBodyString(){
	return (bodyString);
}