/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/27 16:13:32 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp

#include <iostream>
#include <map>
#include "../incs/request.hpp"
#include <sys/socket.h>
#include "ServerData.hpp"

class response{
	private:
		std::string statusLine;
		std::map<std::string, std::string> headers;
		std::string body;
	public:
		void clear(){
			statusLine.clear();
			headers.clear();
			body.clear();
		}
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

		void requestEntityTooLarge(request &req){
			statusLine = req.getHttpV() + " 403 request entity too large";
			body = readFromFile("./pages/403.html");
			headers["Content-Length"] = std::to_string(body.size());
		}
		
		void notImplemented(request &req){
			statusLine = req.getHttpV() + " 501 not implemented";
			body = readFromFile("./pages/501.html");
			headers["Content-Length"] = std::to_string(body.size());
		}
		
		bool isReqWellFormated(request &req, ServerData &server){
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
			// checking the body size
			if ((req.getBodyString()).size() > server.getMaxBodySize()){
				requestEntityTooLarge(req);
				return (false);
			}
			//checking post method
			if (req.getMethod() == 1){
				std::map<std::string, std::string> map = req.getHeadersMap();
				if ((map["Transfer-Encoding"]).size() == 0 && (map["Content-Length"]).size() == 0){
					statusBadRequest(req);
					return (false);
				}
				//checking Transfer-Encoding
				if ((map["Transfer-Encoding"]).size() != 0){
					//501 not implemented
					map["Transfer-Encoding"] = (map["Transfer-Encoding"]).substr(0, (map["Transfer-Encoding"]).size() -1);
					if ((map["Transfer-Encoding"]).compare("chunked") != 0){
						notImplemented(req);
						return (false);
					}
				}
			}
			return (true);
		}
		
		void notFound(request &req){
			statusLine = req.getHttpV() + " 404 not found";
			body = readFromFile("./pages/404.html");
			headers["Content-Length"] = std::to_string(body.size());
		}

		std::string parsUri(const std::string &uri){
			std::string res;
			unsigned int i = uri.size() - 1;
			for(; i != 0; i--){
				if (uri.at(i) != '/')
					break;
			}
			res = uri.substr(0, i + 1);
			res.append("/");
			return (res);
		}

		bool isLocation(request &req, ServerData &server){
			std::string url = parsUri(req.getUrl());
			// req.setUrl(url);
			std::cout << url << std::endl;
			// exit(0);
			std::vector<Location> locs = server.getLocation();
			for(unsigned int i = 0; i < locs.size(); i++){
				if (url.compare(locs[i].path) == 0){
					return (true);
				}
			}
			notFound(req);
			return (false);
		}
		
		bool isLocationHaveRedi(request &req, ServerData &server){
			return (false);
		}
		
		void generate(request &req, ServerData &serv){
			// servs = servers;
			// if (isReqWellFormated(req, serv)){
			// 	if (isLocation(req, serv)){
					//checking if location have redirection
					// if (isLocationHaveRedi(req, serv)){
						
					// }
					// else{
					// 	statusLine = "HTTP/1.1 200 OK";
					// 	body = "hello world!";
					// 	headers["Content-Length"] = "12";
					// }
				// }
			// }
			// else{
				// /checking the location requested
				// statusLine = "HTTP/1.1 200 OK";
				// body = "fuck u!";
				// headers["Content-Length"] = "7";
			// }
			// else	////request is not well formated
			// 	std::cout << "request is not well formated\n";
			// std::cout << req.getMethod() << std::endl;
			// std::cout << req.getUrl() << std::endl;
			statusLine =  "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!";;
			// status code https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3.1
			
		}
		bool get_resources()
		{
			//  ia makanch l file fe root err 
			std::string root = "";
			 

			return true;
		}
		const char *responce(){
			// hna khas tkon responce
			// genrate httml responce <<
		}
		bool handel_get(){
			if(!get_resources())
				return false;
			
			return(true);
		}
		bool handel_delete(){
		

			return true;
		}
		bool handel_post(){
			return true;
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