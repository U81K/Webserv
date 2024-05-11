/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingReq.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 15:48:48 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/07 11:18:27 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>

int main(void){
	std::ifstream requestF("request.txt");
	if (!requestF.is_open()){
		std::cerr << "ERROR: reading the request.txt"  << std::endl;
		exit(1);
	}
	std::string line;
	std::string fullRequest;
	while(std::getline(requestF, line)){
		fullRequest.append(line);
		fullRequest.append("\n");
	}
	//parsing the req
	std::istringstream iss(fullRequest);
	std::vector<std::string> headers;
	std::string headersLine;
	while(std::getline(iss, headersLine)){
		if (headersLine.find("\r\n\r\n")){
			std::cout << "found r n\n";
		}
		else
			headers.push_back(headersLine);
	}
	for(int i = 0; i < headers.size(); i++){
		std::cout << "+" << headers[i] << "+" << std::endl;
	}
	std::string request = "GET /index.html HTTP/1.1\r\n"
						"Host: localhost\r\n"
						"Connection: keep-alive\r\n"
						"Content-Length: 12\r\n"
						"\r\n"
						"Hello World!";
	// std::cout << fullRequest;
	// std::cout << "HELLO\rhi" << std::endl;
	requestF.close();
	return (0);
}

