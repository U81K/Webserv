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
		std::string to_string(int num){
			std::stringstream ss; // Create a stringstream object
    		ss << num; // Insert the integer into the stringstream
    		return(ss.str()); // Extract the string from the stringstream
		};
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
			std::ifstream file(filePath.c_str());
			if (!file.is_open()){
				std::cerr << "unable to open the file: " << filePath << std::endl;
				return ("");
			}
			std::ostringstream ss;
			ss << file.rdbuf();
			return (ss.str());
		}
		
		void statusBadRequest(request &req){
			statusLine = req.getHttpV() + " 400 Bad Request";
			body = readFromFile("./pages/400.html");
			headers["Content-Length"] = to_string(body.size());
		}
		
		void statusrequestUriTooLarge(request &req){
			statusLine = req.getHttpV() + " 414 request-uri too large";
			body = readFromFile("./pages/414.html");
			headers["Content-Length"] = to_string(body.size());
		}

		void requestEntityTooLarge(request &req){
			statusLine = req.getHttpV() + " 403 request entity too large";
			body = readFromFile("./pages/403.html");
			headers["Content-Length"] = to_string(body.size());
		}
		
		void notImplemented(request &req){
			statusLine = req.getHttpV() + " 501 not implemented";
			body = readFromFile("./pages/501.html");
			headers["Content-Length"] = to_string(body.size());
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
			headers["Content-Length"] = to_string(body.size());
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
			if (i == uri.size())
				return ("/");
			if (uri.at(i) == '/'){
				res = uri.substr(0, i);
			}
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
		std::string getFileName(request &req){
			std::string ret = "random";
			size_t fnPos = (req.getBodyString()).find("filename=");
			if (req.isboundaryFound()){
				if (fnPos != std::string::npos){
					std::string ret = (req.getBodyString()).substr(fnPos + 10);
					unsigned int i = 0;
					for (; i < ret.size(); i++){
						if (ret.at(i) == '\"')
							break;
					}
					ret = ret.substr(0, i);
					return (ret);
				}
			}
			return (ret);
		}
		
		std::string getBodyWitoutBound(request &req){
			std::string ret;
			if (req.isboundaryFound()){
				std::string FullBodyString = req.getBodyString();
				std::string boundary = FullBodyString.substr(0, FullBodyString.find("\r\n"));
				size_t contentStart = FullBodyString.find("\r\n\r\n") + 4;
				ret = FullBodyString.substr(contentStart);
				if (ret.find(boundary) != std::string::npos){
					ret = ret.substr(0, ret.find(boundary));
				}
				return (ret);
			}
			return (req.getBodyString());	
		}

		
		bool getResourceType(std::string path){ // return 1 for dir 0 for a file
			struct stat statbuf;
			
			if (stat(path.c_str(), &statbuf) != 0)
				std::cout << path << " does not exist." << std::endl;
			else {
				if (S_ISDIR(statbuf.st_mode))
					std::cout << path << " is a directory." << std::endl;
				else if (S_ISREG(statbuf.st_mode))
					std::cout << path << " is a file." << std::endl;
				else
					std::cout << path << " is neither a file nor a directory." << std::endl;
			}
			return (0);
		}
		
		bool	isFile(std::string filePath){
			struct stat statBuf;

			if (stat(filePath.c_str(), &statBuf) != 0)
        		return false;
    		return S_ISREG(statBuf.st_mode);
		}	
		
		std::string removeLoc(request &req){
			unsigned int i = 1;
			
			std::string fullUri = req.getUrl();
			for (; i < fullUri.size(); i++){
				if (fullUri.at(i) == '/')
					break;
			}
			std::string res;
			if (i == fullUri.size())
				res = fullUri;
			else
				res = fullUri.substr(i, fullUri.size());
			return (res);
		}
		
		std::string generateDirectoryListing(const std::string& directoryPath) {
   			std::vector<std::string> files;
			DIR* dir = opendir(directoryPath.c_str());
			if (dir == NULL) {
				return "";
			}

			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL) {
				std::string filename = entry->d_name;
				if (filename != "." && filename != "..") {
					files.push_back(filename);
				}
			}
			closedir(dir);
			
			std::stringstream listing;
			listing << "<h1>Index of " << directoryPath << "</h1><ul>";
			for (unsigned int i = 0; i < files.size(); ++i) {
				std::string file = files[i];
				listing << "<li><a href=\"" << file << "\">" << file << "</a></li>";
			}
			listing << "</ul></body></html>";
			return (listing.str());
		}
		
		int cgiPost(std::string scriptPath, request &req){
			std::ifstream scriptFile(scriptPath.c_str());
			if (!scriptFile.is_open()){
				std::cerr << "error opening the script file\n";
				return (1);
			}
			int pipeOut[2];
			int pipeIn[2];
			if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1){
				std::cerr << "error piping\n";
				return (1);
			}
			pid_t pid = fork();
			if (pid == 0){
				close(pipeOut[0]); // Close read end of stdout pipe
        		dup2(pipeOut[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        		close(pipeOut[1]);
				
				close(pipeIn[1]); // Close write end
				dup2(pipeIn[0], STDIN_FILENO); // Redirect stdin to pipe read end
				close(pipeIn[0]);
				
				std::string pyPath = "/usr/local/bin/python3";
				char* args[] = {(char *)pyPath.c_str(), (char *)scriptPath.c_str(), NULL};
				char *env[5];
				env[0] = ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
				env[1] = ft_strdup("REQUEST_METHOD=POST");
				std::string bodyLen = "CONTENT_LENGTH=" + to_string((req.getBodyString()).size());
				env[2] = (char*)bodyLen.c_str();//
				std::string scriptFileName = "SCRIPT_FILENAME=" + scriptPath;
				env[3] = (char *)scriptFileName.c_str();
				env[4] = NULL;
				if (execve("/usr/local/bin/python3", args, env) < 0){
					std::cerr << "error execve\n";
					return (-1);
				}
				exit(1);
			}
			else {
				close(pipeOut[1]); // Close write end of stdout pipe
        		close(pipeIn[0]); // Close read end of stdin pipe
				
				// Write the request body to the stdin pipe
        		write(pipeIn[1], (req.getBodyString()).c_str(), (req.getBodyString()).size());
        		close(pipeIn[1]); // Close write end after writing
				
				int status;
				waitpid(pid, &status, 0);
				// Read output from the pipe
				char buffer[4096];
				ssize_t bytesRead;
				// std::string body;
				while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
					buffer[bytesRead] = '\0';
					std::string buf(buffer, bytesRead);
					body = body.append(buf);
				}
				close(pipeOut[0]);
				if (WIFEXITED(status)){
					if (WEXITSTATUS(status) == 0) {
						statusLine = "HTTP/1.1 200 OK";
						headers["Content-Length"] = to_string(body.size());
						headers["Content-Type"] = "text/html";
						return (0);
					} else {
						statusLine = "HTTP/1.1 500 Internal Server Error";
						body = "500 Internal Server Error";
						headers["Content-Length"] = to_string(body.size());
						return (-1);
					}
				}
				else
					return (-1);
        	}
		}
		
		void handlePost(request &req, location loc){
			if ((loc.getDirective("upload_path")).size() > 0){ //location support upload
				// get the file name
				std::string fName = getFileName(req);
				// get the clean body
				std::string bodyWitoutBound;
				bodyWitoutBound = getBodyWitoutBound(req);
				// output the filer
				std::string fullpath = loc.getDirective("upload_path") + "/" + fName;
				std::ofstream file(fullpath.c_str(), std::ios::binary);
				if (!file.is_open()) {
				    std::cerr << "Failed to open file: " << loc.getDirective("upload_path") + "/" + fName << std::endl;
				    return;
				}
				file.write(bodyWitoutBound.data(), bodyWitoutBound.size());
				file.close();
				statusLine = "HTTP/1.1 201 Created";
				headers["Location"] = loc.getDirective("upload_path") + "/" + fName;
				headers["Content-Length"] = "30";
				body = "Resource successfully created.";
			}
			else{
				std::string fullPath = loc.getDirective("root") + removeLoc(req);
				struct stat statbuf;
				// std::string fileContent;
				
				std::cout << fullPath << std::endl;
				if (stat(fullPath.c_str(), &statbuf) != 0){//does not exist
					statusLine = "HTTP/1.1 404 Not Found";
					headers["Content-Length"] = "30";
					body = "404 Not Found from handle post";
				}
				else {
					if (S_ISDIR(statbuf.st_mode)){//is a directory
						//check if the path end with "/"
						if (fullPath.at(fullPath.size() - 1) == '/'){
							//checking if dir has index file
							std::string indexPath = fullPath + "/index.html";
							if (isFile(indexPath)){
								bool isLocationHasCgi = false;
								if (!isLocationHasCgi){
									statusLine = "HTTP/1.1 403 Forbidden";
									body  = "Directory listing is not allowed.";
									headers["Content-Length"] = "33";
								}
							}
							else {
								statusLine = "HTTP/1.1 403 Forbidden";
								body  = "Directory listing is not allowed.";
								headers["Content-Length"] = "33";
							}
						}
						else{
							statusLine = "HTTP/1.1 301 Moved Permanently";
							headers["Location"] = req.getUrl() + "/";
							body = "Moved Permanently";
							headers["Content-Length"] = "17";
						}
					}
					else if (S_ISREG(statbuf.st_mode)){//is a file.
						if (locationHasCgi(fullPath)){
							std::cout << "kyen cgi from post\n";
							cgiPost(fullPath, req);
						}
						else{
							statusLine = "HTTP/1.1 403 Forbidden";
							body = "Forbidden";
							headers["Content-Length"] = "9";
						}
					}
				}
			}
		}
		
		size_t ft_strlen(char *s){
			size_t i = 0;
			while (s[i])
				i++;
			return (i);
		}
		
		char	*ft_strdup(const char *s)
		{
			char	*p;
			int		i;

			p = (char *)malloc(sizeof(char) * ft_strlen((char *)s) + 1);
			if (p == NULL)
				return (0);
			i = 0;
			while (s[i] != '\0')
			{
				p[i] = (char)s[i];
				i++;
			}
			p[i] = '\0';
			return (p);
		}
		
		int cgiGet(std::string scriptPath, std::string query){
			std::ifstream scriptFile(scriptPath.c_str());
			if (!scriptFile.is_open()){
				std::cerr << "error opening the script file\n";
				return (1);
			}
			int pipefd[2];
			if (pipe(pipefd) == -1){
				std::cerr << "error piping\n";
				return (1);
			}
			pid_t pid = fork();
			if (pid == 0){
				close(pipefd[0]); // Close read end
				dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
				close(pipefd[1]);
				std::string pyPath = "/usr/local/bin/python3";
				char* args[] = {(char *)pyPath.c_str(), (char *)scriptPath.c_str(), NULL};
				char *env[5];
				env[0] = ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
				env[1] = ft_strdup("REQUEST_METHOD=GET");
				query = "QUERY_STRING=" + query;
				env[2] = (char*)query.c_str();
				std::string scriptFileName = "SCRIPT_FILENAME=" + scriptPath;
				env[3] = (char *)scriptFileName.c_str();
				env[4] = NULL;
				if (execve("/usr/local/bin/python3", args, env) < 0){
					std::cerr << "error execve\n";
					return (-1);
				}
				exit(1);
			}
			else{
				// Parent process
				close(pipefd[1]); // Close write end
				int status;
				waitpid(pid, &status, 0);
				// Read output from the pipe
				char buffer[4096];
				ssize_t bytesRead;
				// std::string body;
				while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
					buffer[bytesRead] = '\0';
					std::string buf(buffer, bytesRead);
					body = body.append(buf);
				}
				close(pipefd[0]);
				if (WIFEXITED(status)){
					if (WEXITSTATUS(status) == 0) {
						statusLine = "HTTP/1.1 200 OK";
						headers["Content-Length"] = to_string(body.size());
						headers["Content-Type"] = "text/html";
						return (0);
					} else {
						statusLine = "HTTP/1.1 500 Internal Server Error";
						body = "500 Internal Server Error";
						headers["Content-Length"] = to_string(body.size());
						return (-1);
					}
				}
				else
					return (-1);
			}
		}
		
		bool locationHasCgi(std::string fullPath){
			// check if the fullPath has py in the end
			size_t dotPos = fullPath.find_last_of('.');
			if (dotPos == std::string::npos || dotPos == 0) {
				return false;
			}
			std::string extension = fullPath.substr(dotPos + 1);
			if (extension == "py")
				return (true);
			else
				return (false);
		}
// 		bool list_directory(std::string &dir_path) {

//     statusLine = "HTTP/1.1 200 OK";
//     // body += "Index of " + dir_path + "\n\r";
//     DIR *dir = opendir(dir_path.c_str());
//     if (dir == nullptr) {
//         return false;
//     }
//     struct dirent *output;
//     while ((output = readdir(dir)) != NULL) {
//         std::string name = output->d_name;
//         if (name == "." || name == "..") {
//             continue;
            
//         }
//         body += name + "\n";
//     }
//     std::string htmlContent =
//     "<!DOCTYPE html>\n"
//     "<html lang=\"en\">\n"
//     "<head>\n"
//     "    <meta charset=\"UTF-8\">\n"
//     "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
//     "    <title>Directory Listing</title>\n"
//     "    <style>\n"
//     "        body {\n"
//     "            font-family: Arial, sans-serif;\n"
//     "            margin: 20px;\n"
//     "        }\n"
//     "        h1 {\n"
//     "            margin-bottom: 20px;\n"
//     "        }\n"
//     "        ul {\n"
//     "            list-style-type: none;\n"
//     "            padding: 0;\n"
//     "        }\n"
//     "        li {\n"
//     "            margin-bottom: 10px;\n"
//     "        }\n"
//     "        a {\n"
//     "            text-decoration: none;\n"
//     "            color: #007bff;\n"
//     "        }\n"
//     "    </style>\n"
//     "</head>\n"
//     "<body>\n"
//     "    <h1>Directory Listing</h1>\n"
//     "    <ul>\n"
//         + generateListItems(body) +
//     "    </ul>\n"
//     "</body>\n"
//     "</html>\n";
//     closedir(dir);
//     // body += "</ul>\n</body>\n</html>";
//     body = htmlContent;
//     std::cout << body << std::endl;
//     // std::string size = "" + body.size();
//     std::cout << "body = " << body << std::endl;
//     headers["Content-Length"] = std::to_string(body.size());
//     return true;
// }
// bool isLocationHaveRedi(location loc){
//     std::string dir = loc.getDirective("return");
//     if (dir.size() > 0){
//         statusLine = "HTTP/1.1 301 Moved Permanently";
//         headers["Location"] = dir;
//         return (true);
//     }
//     return (false);
// }
// bool get_resources(request & req)
// {
//     root = "www";
//     is_dir = true;
//     auto_index = false ;
//     path = "./" + root + req.getUrl();
//     // (void) req;
// // 	//  ia makanch l file fe root err 
// // 	// std::string root = "";
// // 	root = "/www";
// // 	path = "." + root +req.getUrl();
// // 	std::cout << "\033[36m";
// // 	std::cout << "root path " << path << std::endl;
// // 	// std::string path = "." + req.getUrl();
// // 	struct stat object_stat;
// // 	// object_stat.
// // 	if(stat(path.c_str(),&object_stat) != 0)
// // 		return false;
//     return true;
// }
// 	bool handel_get(request &req, ServerData &serv){
// 		(void) serv;
// 		if(!get_resources(req))
// 		{
// 			notFound(req);
// 			return false;
// 		}
// 		else
// 		{
// 			// std::cout << "\n\n\n\n"<< "bad trip" << req.getUrl() << "\n\n\n\n\n\n\n";
// 			std::string line ;
// 			std::ifstream file_("www/index.html");
// 			if(auto_index){
// 				std::ostringstream file_content;
// 				statusLine = "HTTP/1.1 200 OK";
// 				// file_content << "www/index.html";
// 				while(std::getline(file_,line))
// 				{
// 					body += line + '\n';
// 					std::cout << line << std::endl;

// 				}
// 				headers["Content-Length"] = std::to_string(body.size());
// 				return(true);
// 			}
// 			else if(is_dir)
// 				list_directory(path);
// 			else {
// 					std::ifstream  zeb(path);
// 					std::string line ;
// 					statusLine = "HTTP/1.1 200 OK";
// 					while(getline(zeb,line))
// 						body += line + "\n";
// 			}
// 			headers["Content-Length"] = std::to_string(body.size());    
// 			return(true);
// 		}
// 	}
		void handleGet(request &req, location loc){
			std::string fullPath = loc.getDirective("root") + removeLoc(req);
			struct stat statbuf;
			std::string fileContent;
			
			int questionPos = fullPath.find("?");
			std::string query = "";
			if (questionPos != static_cast<int>(std::string::npos)){
				query = fullPath.substr(questionPos + 1, fullPath.size());
				fullPath = fullPath.substr(0, questionPos);
			}
			// std::cout << "fullPath requested = " << fullPath << std::endl;
			// std::cout << "query = " << query << std::endl;
			if (stat(fullPath.c_str(), &statbuf) != 0){//does not exist
				statusLine = "HTTP/1.1 404 Not Found";
				headers["Content-Length"] = "29";
				body = "404 Not Found from handle get";
			}
			else {
				if (S_ISDIR(statbuf.st_mode)){//is a directory
					//check if the path end with "/"
					if (fullPath.at(fullPath.size() - 1) == '/'){
						//checking if dir has index file
						std::string indexPath = fullPath + "/index.html";
						if (isFile(indexPath)){
							std::string indexContent = readFromFile(indexPath);
							if (!indexContent.empty()) {
								statusLine = "HTTP/1.1 200 OK";
								body = indexContent;
								headers["Content-Length"] = to_string(indexContent.size());
							}
						}
						else if (loc.getDirective("autoIndex").compare("on") == 0){//check if location has autoindex
							std::cout << "kayen auto index\n";
							std::string directoryListing = generateDirectoryListing(fullPath);
							statusLine = "HTTP/1.1 200 OK";
							body = directoryListing;
							headers["Content-Length"] = to_string(body.size());
						}
						else {
							statusLine = "HTTP/1.1 403 Forbidden";
							body  = "Directory listing is not allowed.";
							headers["Content-Length"] = "33";
						}
					}
					else{
						statusLine = "HTTP/1.1 301 Moved Permanently";
						headers["Location"] = req.getUrl() + "/";
						body = "Moved Permanently";
						headers["Content-Length"] = "17";
					}
				}
				else if (S_ISREG(statbuf.st_mode)){//is a file.
					if (locationHasCgi(fullPath)){
						cgiGet(fullPath, query);
					}
					else{
						fileContent = readFromFile(fullPath);
						statusLine = "HTTP/1.1 200 OK";
						body = fileContent;
						headers["Content-Length"] = to_string(fileContent.size());
					}
				}
			}
		}

	bool delete_directory(const std::string &path) {
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

bool handle_delete(request &req , location &loc)
{
   
    std::cout << "handel delete" << std::endl;
    std::string path = loc.getDirective("root") + removeLoc(req);
    struct stat object_stat;
    // object_stat.
    if(stat(path.c_str(),&object_stat) != 0)
        notFound(req);
    else if(S_ISDIR(object_stat.st_mode))//mode_t st_mode: File mode, which includes the file type and file mode bits (permissions).
    {
        // hna khasni nchecky permissions
        if(object_stat.st_mode & S_IWUSR){

            if (delete_directory(path)) {
                statusLine = "HTTP/1.1 200 OK ";
                body = "Directory deleted";
                headers["Content-Length"] = to_string(body.size());
            } else {
                statusLine = "HTTP/1.1 500 Internal Server Error";
                body = "bad trip!";
                headers["Content-Length"] = to_string(body.size());
            }
        }
        else {
            statusLine = "HTTP/1.1 500 Internal Server Error";
            body = "perrmission denied";
            headers["Content-Length"] = to_string(body.size());
        }
    }
    else
    {
        if(object_stat.st_mode & S_IWUSR){
            std::remove(path.c_str());
            statusLine = "HTTP/1.1 200 OK ";
            body = "File deleted";
            headers["Content-Length"] = to_string(body.size());
        }
        else {
            statusLine = "HTTP/1.1 500 Internal Server Error";
            body = "perrmission denied";
            headers["Content-Length"] = to_string(body.size());
        }

    }
    return true;
	}
		
		void generate(request &req, ServerData &serv){
			if (isReqWellFormated(req, serv)){
				if (isLocation(req, serv)){
					location loc = getLoc(req, serv);
					if (!isLocationHaveRedi(loc)){
						if (isMethodAllowed(req, loc)){
							if (req.getMethod() == request::POST)
								handlePost(req, loc);
							else if (req.getMethod() ==request::GET)
								handleGet(req, loc);
							else if(req.getMethod() == request::DELETE)
								handle_delete(req,loc);
							else{ 
								statusLine = "HTTP/1.1 200 OK";
								body = "hello world!";
								headers["Content-Length"] = "12";
							}
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