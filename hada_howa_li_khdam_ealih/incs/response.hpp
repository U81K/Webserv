/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/27 16:13:32 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef response_hpp
#define response_hpp
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include "../incs/request.hpp"
#include <sys/socket.h>
#include "ServerData.hpp"
#include<cstdio>
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

		std::string parsUri(const std::string &uri){
			std::string res;
			unsigned int i = uri.size() - 1;
			for(; i != 0; i--){
				if (uri.at(i) != '/')
					break;
			}
			res = uri.substr(0, i + 1);
			res.append("/");
			return (res);
		}

		bool isLocation(request &req, ServerData &server){
			std::string url = parsUri(req.getUrl());
			// req.setUrl(url);
			std::cout << url << std::endl;
			// exit(0);
			std::vector<Location> locs = server.getLocation();
			for(unsigned int i = 0; i < locs.size(); i++){
				if (url.compare(locs[i].path) == 0){
					return (true);
				}
			}
			notFound(req);
			return (false);
		}
		
		bool isLocationHaveRedi(request &req, ServerData &server){
			return (false);
		}

// The stat structure contains several fields that hold information about a file. These fields include:

//     dev_t st_dev: Device ID of the device containing the file.
//     ino_t st_ino: File serial number, also known as inode number.
//     mode_t st_mode: File mode, which includes the file type and file mode bits (permissions).
//     nlink_t st_nlink: Number of hard links to the file.
//     uid_t st_uid: User ID of the file owner.
//     gid_t st_gid: Group ID of the file.
//     dev_t st_rdev: Device ID (for special files like character or block devices).
//     off_t st_size: File size in bytes. For symbolic links, it represents the length of the pathname contained in the link.
//     time_t st_atime: Time of the last access.
//     time_t st_mtime: Time of the last modification.
//     time_t st_ctime: Time of the last status change.
//     blksize_t st_blksize: Preferred block size for filesystem I/O.
//     blkcnt_t st_blocks: Number of 512-byte blocks allocated to the file.

//     S_ISBLK(m): Checks if the file is a block special file.
//     S_ISCHR(m): Checks if the file is a character special file.
//     S_ISDIR(m): Checks if the file is a directory.
//     S_ISFIFO(m): Checks if the file is a FIFO (named pipe).
//     S_ISREG(m): Checks if the file is a regular file.
//     S_ISLNK(m): Checks if the file is a symbolic link.
//     S_ISSOCK(m): Checks if the file is a socket.

//     S_IRWXU: Read, write, execute/search by the owner.
//         S_IRUSR: Read permission for the owner.
//         S_IWUSR: Write permission for the owner.
//         S_IXUSR: Execute/search permission for the owner.
//     S_IRWXG: Read, write, execute/search by the group.
//         S_IRGRP: Read permission for the group.
//         S_IWGRP: Write permission for the group.
//         S_IXGRP: Execute/search permission for the group.
//     S_IRWXO: Read, write, execute/search by others.
//         S_IROTH: Read permission for others.
//         S_IWOTH: Write permission for others.
//         S_IXOTH: Execute/search permission for others.

// Additionally, there are special mode bits:

//     S_ISUID: Set-user-ID on execution.
//     S_ISGID: Set-group-ID on execution.
//     S_ISVTX: On directories, it restricts deletion (sticky bit).

	// struct dirent ->
    // d_ino: This field represents the inode number of the file. An inode is a data structure that stores metadata about a file, such as permissions, ownership, and file type.
    // d_off: The value stored in d_off is the same as the value returned by the telldir function at the current position in the directory stream. It is often used for tracking positions within the directory.
    // d_reclen: This field indicates the size of the returned record in bytes. It may not match the size of the dirent structure definition, so applications should use the value of d_reclen to determine the size of the record.
    // d_type: This field contains a value indicating the type of the file. It allows applications to avoid the overhead of calling the lstat function if they only need to perform certain actions based on the file type.

    // When certain feature test macros are defined (e.g., _DEFAULT_SOURCE or _BSD_SOURCE), additional macro constants are defined by the glibc library to represent the value stored in d_type. These constants include:
    // DT_BLK: Block device.
    // DT_CHR: Character device.
    // DT_DIR: Directory.
    // DT_FIFO: Named pipe (FIFO).
    // DT_LNK: Symbolic link.
    // DT_REG: Regular file.
    // DT_SOCK: UNIX domain socket.
    // DT_UNKNOWN: The file type could not be determined.
			
			//POSIX https://medium.com/@cloud.devops.enthusiast/posix-59d0ee68b498	
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
			std::cout << "handel delete" << std::endl;
			std::string path = "." + req.getUrl();
			struct stat object_stat;
			if(stat(path.c_str(),&object_stat) != 0)
				notFound(req);
			if(S_ISDIR(object_stat.st_mode))//mode_t st_mode: File mode, which includes the file type and file mode bits (permissions).
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
		void generate(request &req, ServerData &serv){


			
			
			// servs = servers;
			// if (isReqWellFormated(req, serv)){
			// 	if (isLocation(req, serv)){
					//checking if location have redirection
					// if (isLocationHaveRedi(req, serv)){
						
					// }
					// else{
					// 	statusLine = "HTTP/1.1 200 OK";
					// 	body = "hello world!";
					// 	headers["Content-Length"] = "12";
					// }
				// }
			// }
			// else{
				// /checking the location requested
				// statusLine = "HTTP/1.1 200 OK";
				// body = "fuck u!";
				// headers["Content-Length"] = "7";
			// }
			// else	////request is not well formated
			// 	std::cout << "request is not well formated\n";
			// std::cout << req.getMethod() << std::endl;
			// std::cout << req.getUrl() << std::endl;
			// statusLine =  "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!";;
			if(req.getMethod() == request::DELETE)
				handle_delete(req,serv);
			else if(req.getMethod() == request::POST)
				handel_post();
			else if(req.getMethod() == request::GET) 	 
				handel_get();
			else if(req.getMethod() == request::UNKNOWN)
				std::cout << "baaaaaad trip4" << std::endl;			
			// status code https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3.1
			
		}

		bool get_resources()
		{
			//  ia makanch l file fe root err 
			std::string root = "";
			 

			return true;
		}
		const char *responce(){
			// hna khas tkon responce
			// genrate httml responce <<
			return "bad trip";
		}
		bool handel_get(){
			if(!get_resources())
				return false;
			
			return(true);
		}

		bool handel_post(){
			return true;
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