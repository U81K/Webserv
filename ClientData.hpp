/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/11 11:31:15 by bgannoun         ###   ########.fr       */
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

enum class reqMethod {
	NON,
    GET,
    DELETE,
    POST,
};

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
		std::string req;
		bool isReqFinished;
		bool isReadingBody;
		bool isPostReq;
		size_t readed;
		int callCounter;
		size_t reqSize;
		size_t bodySize;
		std::string boundary;
		reqMethod reqMet;
		//
		bool readBodyMode;
		std::string body;
		size_t contentLend;
		std::vector<std::string> headers;
		std::string bodyString;
		int TotalBytesReaded;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
			isReqFinished = false;
			isPostReq = false;
			readed = 0;
			callCounter = 0;
			reqSize = 0;
			bodySize = 0;
			reqMet = reqMethod::NON;
			readBodyMode = false;
			contentLend = -1;
			TotalBytesReaded = 0;
		}
		int getSocketFd(){
			return (sockfd);
		}
		size_t getContLen(){
			std::istringstream iss(req);
			std::string line;
			int contentLength = -1; // Initialize with a default value

			while (std::getline(iss, line)) {
				if (line.find("Content-Length:") != std::string::npos) {
					// Found the Content-Length header, extract its value
					std::istringstream lineStream(line);
					std::string header, value;
					lineStream >> header >> value;
					contentLength = std::stoi(value);
					break;
				}
			}
			return contentLength;
		}
		size_t bodyCounter(){
			size_t emptyLinePos = req.find("\r\n\r\n");
			if (emptyLinePos == std::string::npos) {
				std::cerr << "Empty line separating headers from body not found." << std::endl;
				return -1;
			}
			std::string body = req.substr(emptyLinePos + 4);
			return body.size();
		}
		// bool requestIsFinished(){
		// 	if (!isPostReq){
		// 		if (req.find("GET") != std::string::npos){//checking GET
		// 			if (req.find("\r\n\r\n") != std::string::npos)
		// 				return (true);
		// 		}
		// 		else if (req.find("DELETE") != std::string::npos){
		// 			if (req.find("\r\n\r\n") != std::string::npos)
		// 				return (true);
		// 		}
		// 	}
		// 	if (req.find("POST") != std::string::npos){//checking POST
		// 		isPostReq = true;
		// 		if (bodyCounter() == getContLen())
		// 			return (true);
		// 	}
		// 	return (false);
		// }
		bool requestIsFinished(){
			// std::cout << req << std::endl;
			// if (bodyCounter() == getContLen())
			// 	return (true);
			// std::cout << bodyCounter() << " " << getContLen() << std::endl;
			// std::cout << "conten-len = " << getContLen() << std::endl;
			if (req.find("GET") != std::string::npos){//checking GET
				if (req.find("Content-Length:") != std::string::npos){
					if (req.find("\r\n\r\n") != std::string::npos && bodyCounter() == getContLen())
						return (true);
				}
				else{
					if (req.find("\r\n\r\n") != std::string::npos)
						return (true);
				}
			}
			else if (req.find("DELETE") != std::string::npos){
				if (req.find("Content-Length:") != std::string::npos){
					if (req.find("\r\n\r\n") != std::string::npos && bodyCounter() == getContLen())
						return (true);
				}
				else{
					if (req.find("\r\n\r\n") != std::string::npos)
						return (true);
				}
			}
			else if (req.find("POST") != std::string::npos){//checking POST
				// std::cout << bodyCounter() << " " << getContLen() << std::endl;
				// if (bodyCounter() < getContLen())
				// 	return (false);
	
				// if (readed >= getContLen())
				// 	return (true);
				if (req.find("\r\n\r\n") != std::string::npos && bodyCounter() == getContLen())
					return (true);
			}
			return (false);
		}
				// Function to parse HTTP request headers
		std::vector<std::string> parseRequestHeaders(const std::string& request) {
			std::vector<std::string> headers;
			std::istringstream iss(request);
			std::string line;
			while (std::getline(iss, line) && !line.empty()) {
				headers.push_back(line);
				std::cout << line;
			}
			return headers;
		}

		// Function to parse and process an HTTP request
		void processHTTPRequest(const std::string& request) {
			// Parse request headers
			std::vector<std::string> headers = parseRequestHeaders(request);

			// Process headers
			size_t contentLength = 0;
			std::string boundary;
			for (const std::string& header : headers) {
				if (header.substr(0, 16) == "Content-Length: ") {
					contentLength = std::stoi(header.substr(16));
				} else if (header.substr(0, 16) == "Content-Type: " && header.find("multipart/form-data") != std::string::npos) {
					size_t pos = header.find("boundary=");
					if (pos != std::string::npos) {
						boundary = header.substr(pos + 9); // Length of "boundary="
					}
				}
			}

			// // Read request body based on content length
			// if (contentLength > 0 && !boundary.empty()) {
			// 	// Extract the request body from the received data
			// 	size_t bodyStart = request.find("\r\n\r\n") + 4;
			// 	std::string requestBody = request.substr(bodyStart);

			// 	// Split the request body into parts using the boundary
			// 	std::size_t pos = requestBody.find(boundary);
			// 	while (pos != std::string::npos) {
			// 		// Process each part of the request body
			// 		// You can parse individual parts here
			// 		// For demonstration, we just print the part
			// 		std::string part = requestBody.substr(0, pos);
			// 		std::cout << "Received HTTP request part: " << part << std::endl;
			// 		requestBody.erase(0, pos + boundary.length());
			// 		pos = requestBody.find(boundary);
			// 	}
			// } else {
			// 	std::cout << "No request body found" << std::endl;
			// }

			// Process request based on headers and body
		}
		
		// size_t calculateHeadersSize(std::vector<std::string> &headers){
		// 	size_t size = 0;
		// 	for (int i = 0; i < headers.size(); i++){
		// 		size += headers[i].size();
		// 	}
		// 	return (size + 2);
		// }
		
		// bool readRequest(char *buffer, size_t bytesReceived){
		// 	// std::cout << "+" << buffer << "+" << std::endl;
		// 	// std::cout << "*" << bytesReceived << "*" << std::endl;
		// 	//first we read the headers
		// 	TotalBytesReaded += bytesReceived;
		// 	// std::cout << "bytes readed= " << bytesReaded << std::endl;
		// 	std::istringstream iss(buffer);
		// 	std::string headersLine;
		// 	if (!readBodyMode){
		// 		while(std::getline(iss, headersLine)){
		// 			if (headersLine == "\r"){
		// 				readBodyMode = true;
		// 				break;
		// 			}
		// 			//checking the content type
		// 			size_t clPos = headersLine.find("Content-Length: ");
		// 			if (clPos != std::string::npos){
		// 				clPos += 16;
		// 				contentLend = ::atof(headersLine.substr(16, headersLine.size()).c_str());
		// 				// std::cout << contentLend << std::endl;
		// 				// exit(0);
		// 			}
		// 			headers.push_back(headersLine);
		// 			// bodyString.append(headersLine);
		// 			// bodyString.append("\n");
		// 			// std::cout << headersLine << std::endl;
		// 		}
		// 	}
		// 	// std::string body;
		// 	if (readBodyMode){
		// 		// std::cout << "*******>> read body mode activated\n";
		// 		// appendi mn \r\n
		// 		if (char *BodyStart = strstr(buffer, "\r\n\r\n")){
		// 			BodyStart += 4;
		// 			body.append(BodyStart, strlen(BodyStart));
		// 			// std::cout << BodyStart;
		// 		}
		// 		else{
		// 			body.append(buffer, strlen(buffer));
		// 		}
		// 	}
		// 	// std::cout << body << std::endl;
		// 	if ((readBodyMode && contentLend == -1) || body.size() == contentLend || (contentLend == TotalBytesReaded - bodyString.size() - 2)){
		// 		isReqFinished = true;
		// 		readBodyMode = 0;
		// 		contentLend = -1;
		// 		// std::cout << headers[1];
		// 		// std::cout << body << std::endl;
		// 		return (true);
		// 	}
		// 	return(false);//mazal massalta req
		// }
		
		bool readRequest(char *buffer, size_t bytesReceived){
			std::string buf(buffer, bytesReceived);
			std::cout << buf << std::endl;
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
				// std::cout << "here\n";
				// std::cout << headers.size();
				char *res = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				send(sockfd, res, strlen(res), 0);
				// std::cout << body;
				// outputHTTPRequestToFile(body, "output.txt");
				// std::cout << body.size() << std::endl;
				// std::cout << body << std::endl;
				isReqFinished = false;
				body.clear();
				headers.clear();
				TotalBytesReaded = 0;
				return (true);
			}
			return (false);
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