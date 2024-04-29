/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/29 20:35:36 by bgannoun         ###   ########.fr       */
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
		void sendResponce(){
			if (isReqFinished){
				char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				send(sockfd, res, strlen(res), 0);
				isReqFinished = false;
				std::cout << req << std::endl;
				req.clear();
			}
			// std::cout << "-----> the request is not finished <-----\n";
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