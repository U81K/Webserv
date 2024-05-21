/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/21 18:59:28 by bgannoun         ###   ########.fr       */
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
	public:
		bool isReqWellFormated(request &req){
			///checking if the url contain a char not allowed
			std::string url = req.getUrl();
			
			std::cout << url << std::endl;
			return (true);
		}
		void generate(request &req){
			statusLine = "HTTP/1.1 200 OK";
			body = "Hello, World!";
			headers["Content-Length"] = "13";
			if (isReqWellFormated(req)){
				///checking the location requested
			}
			else	////request is not well formated
				std::cout << "request is not well formated\n";
			// std::cout << req.getMethod() << std::endl;
			// std::cout << req.getUrl() << std::endl;
		}
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