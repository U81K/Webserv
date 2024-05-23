/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/23 17:28:37 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp

#include <iostream>
#include <map>
#include "../incs/request.hpp"
#include <sys/socket.h>

class response{
	private:
		std::string statusLine;
		std::map<std::string, std::string> headers;
		std::string body;
		std::vector<class ServerData> servs;
	public:
		bool foundInAllowedChat(char c){
			if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
				return (true);
			std::string str = ":/?#[]@!$&'()*+,;=";
			if (str.find(c) != std::string::npos)
				return (true);
			return (false);
		}
		std::string readFromFile(const std::string &filePath){
			std::ifstream file(filePath);
			if (!file.is_open()){
				std::cerr << "unable to open the file: " << filePath << std::endl;
			}
			std::ostringstream ss;
			ss << file.rdbuf();
			return (ss.str());
		}
		
		void statusBadRequest(request &req){
			statusLine = req.getHttpV() + " 400 Bad Request";
			body = readFromFile("./pages/400.html");
			headers["Content-Length"] = std::to_string(body.size());
		}
		
		void statusrequestUriTooLarge(request &req){
			statusLine = req.getHttpV() + " 414 request-uri too large";
			body = readFromFile("./pages/414.html");
			headers["Content-Length"] = std::to_string(body.size());
		}

		bool isReqWellFormated(request &req){
			///checking if the url contain a char not allowed
			std::string url = req.getUrl();
			for (unsigned int i = 0; i < url.size(); i++){
				if (!foundInAllowedChat(url.at(i))){
					statusBadRequest(req);
					return (false);
				}
			}
			//checking the len of uri
			if (url.size() > 2048){
				statusrequestUriTooLarge(req);
				return (false);
			}
			//
			return (true);
		}
		void generate(request &req){
			// servs = servers;
			statusLine = "HTTP/1.1 200 OK";
			body = "Hello, World!";
			headers["Content-Length"] = "13";
			if (isReqWellFormated(req)){
				///checking the location requested
			}
			// else	////request is not well formated
			// 	std::cout << "request is not well formated\n";
			// std::cout << req.getMethod() << std::endl;
			// std::cout << req.getUrl() << std::endl;
		}
		// void generate(request &req, std::vector<class ServerData> &servers){
		// 	servs = servers;
		// 	statusLine = "HTTP/1.1 200 OK";
		// 	body = "Hello, World!";
		// 	headers["Content-Length"] = "13";
		// 	if (isReqWellFormated(req)){
		// 		///checking the location requested
		// 	}
		// 	// else	////request is not well formated
		// 	// 	std::cout << "request is not well formated\n";
		// 	// std::cout << req.getMethod() << std::endl;
		// 	// std::cout << req.getUrl() << std::endl;
		// }
		void sending(int cltFd){
			std::ostringstream response;
			response << statusLine << "\r\n";
			for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
				response << it->first << ": " << it->second << "\r\n";
       		}
			response << "\r\n";
			response << body;
			std::string res = response.str();
			send(cltFd, res.c_str(), res.size(), 0);
		}
};

#endif