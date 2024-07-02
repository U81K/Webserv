/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/09 23:36:12 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp

#include <iostream>
#include <map>
#include <fstream>
#include "../incs/request.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include "ServerData.hpp"
#include <dirent.h>

class responce{
	private:
		bool is_dir ;
		bool auto_index ;
		std::string statusLine;
		std::map<std::string, std::string> headers;
		std::string body;
		std::string path;
		std::string root;
	public:
		void clear();
		bool foundInAllowedChat(char c);
		std::string readFromFile(const std::string &filePath);
		void statusBadRequest(request &req);
		void statusrequestUriTooLarge(request &req);
		void requestEntityTooLarge(request &req);
		void notImplemented(request &req);
		bool isReqWellFormated(request &req, ServerData &server);
		void notFound(request &req);
		std::string getLocation(const std::string &uri);
		bool isLocation(request &req, ServerData &server);
		bool isLocationHaveRedi(location loc);
		location getLoc(request &req, ServerData &server);
		bool isMethodAllowed(request &req, location loc);
		std::string generateListItems(const std::string &fileAndDirNames);
		bool delete_directory(const std::string &path);
		bool handle_delete(request &req, ServerData &serv);
		bool list_directory(std::string &dir_path) ;
		bool get_resources(request & req);
		bool handel_get(request &req, ServerData &serv);
		void generate(request &req, ServerData &serv);
		void sending(int cltFd);
};

#endif