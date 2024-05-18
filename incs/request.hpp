/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 19:04:37 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/18 09:47:14 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef request_hpp
#define request_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

class request{
	public:
		enum Method {
			GET,
			POST,
			DELETE,
			UNKNOWN
		};
		request();
		void addBuffer(std::string &buff, size_t bytesRec);
		bool isReqFinished();
		void printFullReq();
		void clear();
		void parsingRequest();
	private:
		std::vector<std::string> fullReq;
		size_t addBufCouter;
		std::string headersString;
		bool isChunked;
		std::string boundary;
		bool boundaryFound;
		size_t contentLend;
		size_t bytesReaded;
		///
		Method method;//check
		std::string url;//check
		std::string httpVersion;//check
		std::map<std::string, std::string> headersMap;//check
		std::string bodyString;//check
};

#endif