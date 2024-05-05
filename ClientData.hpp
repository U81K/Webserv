/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/29 23:44:55 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientdata_hpp
#define clientdata_hpp

#include <iostream>

class reqData{
	private:
		std::string method;
		std::string url;
		std::string buffer;
	public:
};

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		std::string req;
		bool isReqFinished;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
			isReqFinished = false;
		}
		int getSocketFd(){
			return (sockfd);
		}
		bool requestIsFinished(char *buffer, int bytesReceived){
			std::string req = std::string(buffer);
			// std::cout << req << std::endl;
			if (req.find("GET") != std::string::npos){//checking GET
				if (req.find("\r\n\r\n") != std::string::npos)
					return (true);
			}
			else if (req.find("POST") != std::string::npos){//checking POST
				size_t conLenght = 0;
				char *headerEnd = strstr(buffer, "\r\n\r\n");
				if (headerEnd == NULL)
					return (false);
				size_t clPos = req.find("Content-Length: ");
				if (clPos != std::string::npos){
					std::string line = req.substr(clPos, req.size());
					size_t untelLine = line.find("\n");
					std::string allLine = line.substr(0, untelLine);
					std::string contLenght = allLine.substr(16, allLine.size());
					char *end;
					size_t contLen = std::strtod(contLenght.c_str(), &end);/// Content-Length
					std::string body = headerEnd + 4;
					if (body.size() == contLen){
						std::cout << "----------------------> salat\n";
						return (true);
					}
					else
						return (false);
				}
			}
			return (false);
		}
		bool readRequest(char *buffer, size_t bytesReceived){
			//ila salat request return true
			req.append(buffer);
			if(requestIsFinished((char *)req.c_str(), bytesReceived)){
				isReqFinished = true;
				return (true);
			}
			return(false);// mazal massalta req
		}
		
		void sendResponce(){
			if (isReqFinished){
				const char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				send(sockfd, res, strlen(res), 0);
				isReqFinished = false;
				std::cout << req << std::endl;
				req.clear();
			}
		}

		void finishReq(char *buffer){
			req.append(buffer);
			isReqFinished = true;
		}
		void appendReq(char *buffer){
			req.append(buffer);
		}
};

#endif