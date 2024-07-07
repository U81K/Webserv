/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khaimer <khaimer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/07/05 19:56:43 by khaimer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp

#include <iostream>
#include <map>
#include "../incs/request.hpp"
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include "ServerData.hpp"
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include<cstring>

class response{
	private:
		std::string to_string(int num);
		std::string statusLine;
		std::map<std::string, std::string> headers;
		std::string body;
		// bool is_dir;
		bool auto_index;
		std::string path;
		std::string query_string;
		typedef enum  STATUS_CODE {
			// NotFound,
			// Forbidden,
			// perrmission_denied,

		} STATUS_CODE ;
		typedef struct ultimate{
		bool badtrip;
		bool is_file;
		bool is_dir;
		bool read_per;
		bool exec_per;
		bool write_per;
		} ultimate;
		ultimate mode;
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
		std::string getFileName(request &req);
		std::string getBodyWitoutBound(request &req);
		bool getResourceType(std::string path);
		bool	isFile(std::string filePath);
		std::string removeLoc(request &req);
		int cgiPost(std::string scriptPath, request &req);
		void handlePost(request &req, location loc);
		size_t ft_strlen(char *s);
		char	*ft_strdup(const char *s);
		int cgiGet(std::string scriptPath, std::string query);
		bool locationHasCgi(std::string fullPath);
		std::string generateListItems(const std::string &fileAndDirNames);
		bool list_directory(std::string &dir_path);
		void Forbidden();
		void moved_permanently(request req);
		ultimate info(std::string res_path);
		bool get_resources(request & req,location loc);
		bool handel_get(request &req, location loc);
		bool delete_directory(const std::string &path);
		void perrmission_denied();
		bool handle_delete(request &req , location &loc);
		void generate(request &req, ServerData &serv);
		void sending(int cltFd);
		void responce_( STATUS_CODE status);
};

#endif