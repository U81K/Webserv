/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/16 16:26:59 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientdata_hpp
#define clientdata_hpp

#include <iostream>
#include <fstream>
#include <vector>
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
			bytesReaded = 0;
		}
		void addBuffer(std::string &buff, size_t bytesRec){
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
				//check if the data is checked
				if (headersString.find("Transfer-Encoding: chunked") != std::string::npos){
					std::cout << "the data is chunked\n";
					isChunked = true;
				}
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
		bool isReqFinished(){
			if (method == GET || method == DELETE)
				return (true);
			else if (isChunked){
				for (int i = 0; i < fullReq.size(); i++){
					if (fullReq[i].find("0\r\n\r\n") != std::string::npos)
						return (true);
				}
			}
			else if (contentLend > 0){
				if (boundaryFound){
					for (int i = 0; i < fullReq.size(); i++){
						if (fullReq[i].find(boundary) != std::string::npos){
							// std::cout << boundary << std::endl;
							return (true);
						}
					}
				}
				else if(contentLend == (bytesReaded - 4 - headersString.size()))///there is no boundary
					return (true);
			}
			else if (contentLend == 0)
				return (true);
			return (false);
		}
		void printFullReq(){
			for(int i = 0; i < fullReq.size(); i++){
				std::cout << fullReq[i];
			}
			std::cout << std::endl;
		}
		void clear() {
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
	private:
		std::vector<std::string> fullReq;
		Method method;
		size_t addBufCouter;
		std::string headersString;
		bool isChunked;
		std::string boundary;
		bool boundaryFound;
		size_t contentLend;
		size_t bytesReaded;
};

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		bool isReqFinished;
		request req;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
			isReqFinished = false;
		}
		int getSocketFd(){
			return (sockfd);
		}

		bool readRequest(char *buffer, size_t bytesReceived){
			std::string buff(buffer, bytesReceived);
			req.addBuffer(buff, bytesReceived);
			if (req.isReqFinished()){
				isReqFinished = true;
				return (true);
			}
			return (false);
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
				req.printFullReq();
				req.clear();
				isReqFinished = false;
				return (true);
			}
			return (false);
		}
};

#endif