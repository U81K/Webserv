/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/10 21:34:03 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp

#include <iostream>
#include <map>
#include "../incs/request.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include "ServerData.hpp"
#include <dirent.h>

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
		
		std::string getLocation(const std::string &uri){
			if (uri.size() == 1 && uri.compare("/") == 0){
				return (uri);
			}
			std::string res;
			unsigned int i = 1;
			for (; i < uri.size(); i++){
				if (uri.at(i) == '/')
					break;
			}
			res = uri.substr(0, i);
			if (res.size() > 1)
				res.append("/");
			return (res);
		}

		bool isLocation(request &req, ServerData &server){
			std::string url = getLocation(req.getUrl());
			// std::cout << url << std::endl;
			std::vector<location> locs = server.getLocs();
			for(unsigned int i = 0; i < locs.size(); i++){
				if (url.compare(locs[i].getPath()) == 0){
					return (true);
				}
			}
			notFound(req);
			return (false);
		}
		
		bool isLocationHaveRedi(location loc){
			std::string dir = loc.getDirective("return");
			if (dir.size() > 0){
				statusLine = "HTTP/1.1 301 Moved Permanently";
				headers["Location"] = dir;
				return (true);
			}
			return (false);
		}
		
		location getLoc(request &req, ServerData &server){
			std::string url = getLocation(req.getUrl());
			std::vector<location> locs = server.getLocs();
			for(unsigned int i = 0; i < locs.size(); i++){
				if (url.compare(locs[i].getPath()) == 0){
					return (locs[i]);
				}
			}
			return (location());
		}
		
		bool isMethodAllowed(request &req, location loc){
			std::string method;
			if (req.getMethod() == 0)
				method = "GET";
			else if (req.getMethod() == 1)
				method = "POST";
			else if (req.getMethod() == 2)
				method = "DELETE";
			else if (req.getMethod() == 3)
				method = "UNKNOWN";
			std::string allowedMethods = loc.getDirective("acceptedMethods");
			if ((allowedMethods.find(method) != std::string::npos)){
				return (true);
			}
			statusLine = "HTTP/1.1 405 Method Not Allowed";
			body = "method not allowed";
			headers["Content-Length"] = "18";
			return (false);
		}
		
		bool delete_directory(const std::string &path) 
		{
			//https://www.ibm.com/docs/bg/zos/2.4.0?topic=functions-opendir-open-directory
			//https://medium.com/@noransaber685/exploring-directory-operations-opendir-readdir-and-closedir-system-calls-a8fb1b6e67bb
			DIR *dir = opendir(path.c_str());
			if (!dir) {
				std::cerr << "Failed to open directory: " << path << std::endl;
				return false;
			}
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL){
				if (std::strcmp(entry->d_name, ".") && std::strcmp(entry->d_name, "..")) {
					std::string full_path = path + "/" + entry->d_name;
					struct stat st;//http://codewiki.wikidot.com/c:system-calls:stat
					if (stat(full_path.c_str(), &st) == 0) {
						if (S_ISDIR(st.st_mode)) {
							if (!delete_directory(full_path)) {
								closedir(dir);
								return false;
							}
						} else {
							if (std::remove(full_path.c_str()) != 0) {
								std::cerr << "Failed to remove file: " << full_path << std::endl;
								closedir(dir);
								return false;
							}
						}
					}
				}
			}
			closedir(dir);
			std::remove(path.c_str());
			return true;
		}
		
		bool handle_delete(request &req, ServerData &serv)
		{
			(void) serv;
			std::cout << "handel delete" << std::endl;
			std::string path = "." + req.getUrl();
			struct stat object_stat;
			// object_stat.
			if(stat(path.c_str(),&object_stat) != 0)
				notFound(req);
			else if(S_ISDIR(object_stat.st_mode))//mode_t st_mode: File mode, which includes the file type and file mode bits (permissions).
			{
				// hna khasni nchecky permissions 
				if (delete_directory(path)) {
					statusLine = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\n good trip!";
				} else {
					statusLine = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\nConnection: close\r\n\r\n bad trip !";
				}
			}
			else
			{
				std::remove(path.c_str());
				std::cout << "zeb" << std::endl;
				statusLine = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\n good trip!";
			}
			return true;
		}
		
		bool list_directory(std::string &dir_path) {
			DIR *dir = opendir(dir_path.c_str());
			if (dir == nullptr) {
				return false;
			}
			struct dirent *output;
			while ((output = readdir(dir)) != NULL) {
				std::string name = output->d_name;
				if (name == "." || name == "..") {
					continue;
				}
				statusLine += "<li><a href=\"" + name + "\">" + name + "\n";
			}
			closedir(dir);
			statusLine += "</ul>\n</body>\n</html>";

			std::cout << statusLine << std::endl;
			return true;
		}
		
		bool get_resources()
		{
			//  ia makanch l file fe root err 
			std::string root = "";
			return true;
		}
		
		bool handel_get(request &req, ServerData &serv){
			(void) serv;
			if(!get_resources())
				notFound(req);
			std::string file = "." + req.getUrl();
			list_directory(file);
			return(true);
		}

		void handlePost(request &req, location loc){
			(void) req;
			if ((loc.getDirective("upload_path")).size() > 0){ // location support upload
				// std::cout << loc.getDirective("upload_path") << std::endl;
			}
		}
		
		void generate(request &req, ServerData &serv){
			if (isReqWellFormated(req, serv)){
				if (isLocation(req, serv)){
					location loc = getLoc(req, serv);
					if (!isLocationHaveRedi(loc)){
						if (isMethodAllowed(req, loc)){
							// std::cout << req.getBodyString() << std::endl;
							if (req.getMethod() == 1)
								handlePost(req, loc);
							else{
								statusLine = "HTTP/1.1 200 OK";
								body = "hello world!";
								headers["Content-Length"] = "12";
							}
							// if(req.getMethod() == request::DELETE)
							// 	handle_delete(req,serv);
							// else if(req.getMethod() == request::GET){
							// 	handel_get(req,serv);
							// else if(req.getMethod() == request::POST)
							// 	handel_post();
								// exit(0);
							// }
						}
					}
				}
			}
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