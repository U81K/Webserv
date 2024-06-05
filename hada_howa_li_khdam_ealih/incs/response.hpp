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
		
// 		The stat Structure

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

// File Mode and Permissions

// The st_mode field in the stat structure contains information about the file type and the file mode (permissions). The file type can be checked using specific macros:

//     S_ISBLK(m): Checks if the file is a block special file.
//     S_ISCHR(m): Checks if the file is a character special file.
//     S_ISDIR(m): Checks if the file is a directory.
//     S_ISFIFO(m): Checks if the file is a FIFO (named pipe).
//     S_ISREG(m): Checks if the file is a regular file.
//     S_ISLNK(m): Checks if the file is a symbolic link.
//     S_ISSOCK(m): Checks if the file is a socket.

// File Mode Bits

// These bits define the read, write, and execute permissions for the file's owner, group, and others:

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

// Advanced File Types and Macros

// Some systems may implement advanced file types, such as message queues, semaphores, and shared memory objects. Macros to test these types include:

//     S_TYPEISMQ(buf): Checks if the file is a message queue.
//     S_TYPEISSEM(buf): Checks if the file is a semaphore.
//     S_TYPEISSHM(buf): Checks if the file is a shared memory object.
//     S_TYPEISTMO(buf): Checks if the file is a typed memory object.
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
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
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
    std::string path = "." + req.getUrl();
    if (delete_directory(path)) {
        std::remove(path.c_str());
        statusLine = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\n good trip!";
    } else {
        statusLine = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\nConnection: close\r\n\r\n bad trip ";
    }
    return true;
}
	// 	bool delete_diractory(std::string &path){

	// 		DIR *dir = opendir(path.c_str());
	// 		// std::cout << "dir buf = " << dir->__dd_buf <<std::endl;
	// 		struct dirent *read_dir;
	// 		while((read_dir = readdir(dir)) != NULL)
	// 		{
	// 			if(strcmp(read_dir->d_name,".") && strcmp(read_dir->d_name,".."))
	// 			{
	// 				std::string file;
	// 				file = path + "/" + read_dir->d_name;
	// 				std::cout << "filles to remove " << file << std::endl;
	// 				std::remove(path.c_str());
	// 			}
	// 		}
	// 		return true;
	// }
	// 	bool handel_delete(request &req, ServerData &serv){
	// 		// std::vector<location> locs = serv.getLocs();
	// 		// std::cout << locs[0].getPath() << std::endl;
	// 		// std::cout << req.getUrl() << std::endl;
	// 		// check
	// 		// exit(0);
	// 		// std::cout << "body tring = " <<req.getBodyString() << std::endl;
	// 		// std::cout << "url = "<<req.getUrl() << std::endl;
	// 		// stat https://www.geeksforgeeks.org/how-to-check-a-file-or-directory-exists-in-cpp/

			
	// 		std::string path = "." + req.getUrl();
	// 		delete_diractory(path);
	// 		std::remove(path.c_str());
	// 		statusLine = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\n bad trip!";
	// 		// std::cout << "content = " << req.getContentLen() << std::endl;
	// 		// khasni nchof wach file ola dir 
	// 		// bad trip :D
	// 		// std::cout << req.getHeadersMap

	// 		return true;
	// 	}
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
				std::cout << "baaaaaad trip2" << std::endl;
			else if(req.getMethod() == request::GET) 	 
				std::cout << "baaaaaad trip3" << std::endl;
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