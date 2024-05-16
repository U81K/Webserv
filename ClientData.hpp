/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/16 10:58:38 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientdata_hpp
#define clientdata_hpp

#include <iostream>
#include <fstream>
#include <vector>

class reqData{
	private:
		std::string method;
		std::string url;
		std::string buffer;
	public:
};

#include <sstream>
	
class request{
	public:
		enum Method {
			GET,
			POST,
			DELETE,
			UNKNOWN
		};
		request(){
			method = UNKNOWN;
			addBufCouter = 0;
			isChunked = false;
			boundaryFound = false;
			contentLend = -1;
		}
		void addBuffer(std::string &buff){
			///get the method and content counte
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
				//check if the data is checked
				if (headersString.find("Transfer-Encoding: chunked") != std::string::npos){
					std::cout << "the data is chunked\n";
					isChunked = true;
				}
				//check if theres a boudary
				size_t bouPos = headersString.find("boundary=");
				if (bouPos != std::string::npos){
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
				std::cout << boundary << std::endl;
				std::cout << contentLend << std::endl;
				addBufCouter++;
			}
			fullReq.push_back(buff);
			printFullReq();
		}
		bool isReqFinished(){
			
		}
		void printFullReq(){
			for(int i = 0; i < fullReq.size(); i++){
				std::cout << fullReq[i];
			}
			std::cout << std::endl;
		}	
	private:
		std::vector<std::string> fullReq;
		Method method;
		size_t addBufCouter;
		std::string headersString;
		bool isChunked;
		std::string boundary;
		bool boundaryFound;
		size_t contentLend;
};

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		// std::string req;
		bool isReqFinished;
		bool isReadingBody;
		bool isPostReq;
		size_t readed;
		int callCounter;
		size_t reqSize;
		size_t bodySize;
		std::string boundary;
		//
		bool readBodyMode;
		std::string body;
		double contentLend;
		std::vector<std::string> headers;
		std::string headersString;
		std::string bodyString;
		int TotalBytesReaded;
		size_t bodyStart;
		//
		request req;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
			isReqFinished = false;
			isPostReq = false;
			readed = 0;
			callCounter = 0;
			reqSize = 0;
			bodySize = 0;
			readBodyMode = false;
			contentLend = -1;
			TotalBytesReaded = 0;
			bodyStart = -1;
		}
		int getSocketFd(){
			return (sockfd);
		}

		bool readRequest(char *buffer, size_t bytesReceived){
			std::string buff(buffer, bytesReceived);
			req.addBuffer(buff);
			return (req.isReqFinished());
		}
		
		void outputHTTPRequestToFile(const std::string& httpRequest, const std::string& filename) {
			// Open the output file
			std::ofstream outFile(filename);
			if (!outFile.is_open()) {
				std::cerr << "Failed to open file: " << filename << std::endl;
				return;
			}
			// Write the HTTP request string to the file
			outFile << httpRequest;
			outFile.close();
		}
		
		bool sendResponce(){
			if (isReqFinished){
				char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				send(sockfd, res, strlen(res), 0);
				std::cout << "+" << body << "+" << std::endl;
				isReqFinished = false;
				body.clear();
				headers.clear();
				headersString.clear();
				bodyString.clear();
				boundary.clear();
				TotalBytesReaded = 0;
				return (true);
			}
			return (false);
		}
		
		// void finishReq(char *buffer){
		// 	req.append(buffer);
		// 	isReqFinished = true;
		// }
		
		// void appendReq(char *buffer){
		// 	req.append(buffer);
		// }
};

#endif