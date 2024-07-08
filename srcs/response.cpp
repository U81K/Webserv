/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:42 by bgannoun          #+#    #+#             */
/*   Updated: 2024/07/08 19:08:42 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/response.hpp"
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

std::string response::to_string(int num)
{
    std::stringstream ss; // Create a stringstream object
    ss << num; // Insert the integer into the stringstream
    return(ss.str()); // Extract the string from the stringstream
}
bool response::foundInAllowedChat(char c){
	if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		return (true);
	std::string str = ":/?#[]@!$&'()*+,;=";
	if (str.find(c) != std::string::npos)
		return (true);
	return (false);
}
std::string response::readFromFile(const std::string &filePath){
	std::ifstream file(filePath.c_str());
	if (!file.is_open()){
		std::cerr << "unable to open the file: " << filePath << std::endl;
		return ("");
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}
		
void response::statusBadRequest(request &req){
	statusLine = req.getHttpV() + " 400 Bad Request";
	body = readFromFile("./pages/400.html");
	headers["Content-Length"] = to_string(body.size());
}
		
void response::statusrequestUriTooLarge(request &req){
	statusLine = req.getHttpV() + " 414 request-uri too large";
	body = readFromFile("./pages/414.html");
	headers["Content-Length"] = to_string(body.size());
}

void response::requestEntityTooLarge(request &req){
	statusLine = req.getHttpV() + " 403 request entity too large";
	body = readFromFile("./pages/403.html");
	headers["Content-Length"] = to_string(body.size());
}
		
void response::notImplemented(request &req){
	statusLine = req.getHttpV() + " 501 not implemented";
	body = readFromFile("./pages/501.html");
	headers["Content-Length"] = to_string(body.size());
}

bool response::isReqWellFormated(request &req, ServerData &server){
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
		
void response::notFound(request &req){
	statusLine = req.getHttpV() + " 404 not found";
	body = readFromFile("./Pages/404.html");
	headers["Content-Length"] = to_string(body.size());
}

std::string response::getLocation(const std::string &uri){
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

bool response::isLocation(request &req, ServerData &server){
	std::string url = getLocation(req.getUrl());
	// std::cout << url << std::endl;
	std::vector<location> locs = server.getLocs();
	// std::cout << locs.size() << std::endl;
	// std::cout << locs[0].getPath() << std::endl;
	for(unsigned int i = 0; i < locs.size(); i++){
		if (url.compare(locs[i].getPath()) == 0){
			return (true);
		}
	}
	// std::cout<< "here" << std::endl;
	notFound(req);
	return (false);
}

bool response::isLocationHaveRedi(location loc){
	std::string dir = loc.getDirective("return");
	if (dir.size() > 0){
		statusLine = "HTTP/1.1 301 Moved Permanently";
		headers["Location"] = dir;
		return (true);
	}
	return (false);
}

location response::getLoc(request &req, ServerData &server){
	std::string url = getLocation(req.getUrl());
	std::vector<location> locs = server.getLocs();
	for(unsigned int i = 0; i < locs.size(); i++){
		if (url.compare(locs[i].getPath()) == 0){
			return (locs[i]);
		}
	}
	return (location());
}
		
bool response::isMethodAllowed(request &req, location loc){
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
std::string response::getFileName(request &req){
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
		
std::string response::getBodyWitoutBound(request &req){
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

		
bool response::getResourceType(std::string path){ // return 1 for dir 0 for a file
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

bool	response::isFile(std::string filePath){
	struct stat statBuf;

	if (stat(filePath.c_str(), &statBuf) != 0)
		return false;
	return S_ISREG(statBuf.st_mode);
}	

std::string response::removeLoc(request &req){
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

void response::Internal_Server_Error()
{
	statusLine = "HTTP/1.1 500 Internal Server Error";
	body = readFromFile("./Pages/500.html");
	headers["Content-Length"] = to_string(body.size());

}

void response::OK()
{
	statusLine = "HTTP/1.1 200 OK";
	body = readFromFile("./Pages/200.html");
	headers["Content-Length"] = to_string(body.size());
	headers["Content-Type"] = "text/html";

}

int response::cgiPost(std::string scriptPath, request &req){
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
	int startTime = time(0);
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
		int result;
		while ((result = waitpid(pid, &status, WNOHANG)) == 0){
			if (result == 0){
				if ((time(0) - startTime) > 2){
					Internal_Server_Error();
					kill(pid, SIGKILL);
					close(pipeOut[0]);
					return (1);
				}
			}
		}
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
				headers["Content-Length"] = std::to_string(body.size());
				headers["Content-Type"] = "text/html";
				return (0);
			} else {
				// statusLine = "HTTP/1.1 500 Internal Server Error";
				// headers["Content-Length"] = "0";
				Internal_Server_Error();
				return (-1);
			}
		}
		else
			return (-1);
	}
}

void response::OK_CREATED(location loc,std::string fName)
{
		statusLine = "HTTP/1.1 201 Created";
		headers["Location"] = loc.getDirective("upload_path") + "/" + fName;
		headers["Content-Length"] = "30";
		body = "Resource successfully created.";
}

void response::Moved_Permanently(request req)
{
	statusLine = "HTTP/1.1 301 Moved Permanently";
	headers["Location"] = req.getUrl() + "/";
	body = readFromFile("./Pages/301.html");
	headers["Content-Length"] = "17";
}

void response::handlePost(request &req, location loc){
	std::string fullPath = loc.getDirective("root") + removeLoc(req);
	if (!locationHasCgi(fullPath)){
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
			OK_CREATED(loc,fName);
		}	
	}
	else{
		struct stat statbuf;
		// std::string fileContent;
		
		std::cout << fullPath << std::endl;
		if (stat(fullPath.c_str(), &statbuf) != 0)//does not exist
			notFound(req);
		else {
			if (S_ISDIR(statbuf.st_mode)){//is a directory
				//check if the path end with "/"
				if (fullPath.at(fullPath.size() - 1) == '/'){
					//checking if dir has index file
					std::string indexPath = fullPath + "/index.html";
					if (isFile(indexPath)){
						bool isLocationHasCgi = false;
						if (!isLocationHasCgi)
							Forbidden();
					}
					else
						Forbidden();
				}
				else
					Moved_Permanently(req);
			}
			else if (S_ISREG(statbuf.st_mode)){//is a file.
				if (locationHasCgi(fullPath)){
					cgiPost(fullPath, req);
				}
				else
					Forbidden();
			}
		}
	}
}

size_t response::ft_strlen(char *s){
	size_t i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*response::ft_strdup(const char *s)
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

int response::cgiGet(std::string scriptPath, std::string query){
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
	int startTime = time(0);
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
		int result;
		while ((result = waitpid(pid, &status, WNOHANG)) == 0){
			if (result == 0){
				if ((time(0) - startTime) > 2){
					Internal_Server_Error();
					kill(pid, SIGKILL);
					close(pipefd[0]);
					return (1);
				}
			}
		}
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
			if (WEXITSTATUS(status) == 0){
				statusLine = "HTTP/1.1 200 OK";
				headers["Content-Length"] = std::to_string(body.size());
				headers["Content-Type"] = "text/html";
				return (0);
			}
			else{
				Internal_Server_Error();
			}
		}
		else
			return (-1);
	}
	return (0);
}
void response::clear(){
	statusLine.clear();
	headers.clear();
	body.clear();
}
bool response::locationHasCgi(std::string fullPath){
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
std::string response::generateListItems(const std::string &fileAndDirNames) {
	std::string Items;
	std::istringstream Toread(fileAndDirNames);
	std::string Name;
	
	while (std::getline(Toread,Name)) 
	{
		std::string CompletPath = Name;
		Items += "<li><a href=\"" + CompletPath + "\">" + CompletPath + "</a></li>\n";
	}
	return Items;
}
bool response::list_directory(std::string &dir_path) {

	statusLine = "HTTP/1.1 200 OK";
	// body += "Index of " + dir_path + "\n\r";
	DIR *dir = opendir(dir_path.c_str());
	if (dir == NULL) {
		return false;
	}
	struct dirent *output;
	while ((output = readdir(dir)) != NULL) {
		std::string name = output->d_name;
		if (name == "." || name == "..") {
			continue;
		}
		body += name + "\n";
	}
	std::string htmlContent =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
	"    <title>Directory Listing</title>\n"
	"    <style>\n"
	"        body {\n"
	"            font-family: Arial, sans-serif;\n"
	"            margin: 20px;\n"
	"        }\n"
	"        h1 {\n"
	"            margin-bottom: 20px;\n"
	"        }\n"
	"        ul {\n"
	"            list-style-type: none;\n"
	"            padding: 0;\n"
	"        }\n"
	"        li {\n"
	"            margin-bottom: 10px;\n"
	"        }\n"
	"        a {\n"
	"            text-decoration: none;\n"
	"            color: #007bff;\n"
	"        }\n"
	"    </style>\n"
	"</head>\n"
	"<body>\n"
	"    <h1>Directory Listing</h1>\n"
	"    <ul>\n"
		+ generateListItems(body) +
	"    </ul>\n"
	"</body>\n"
	"</html>\n";
	closedir(dir);
	// body += "</ul>\n</body>\n</html>";
	body = htmlContent;
	// std::cout << body << std::endl;
	// std::string size = "" + body.size();
	// std::cout << "body = " << body << std::endl;
	headers["Content-Length"] = to_string(body.size());
	return true;
}	
void response::Forbidden()
{
	statusLine = "HTTP/1.1 403 Forbidden";
	body  = readFromFile("./Pages/403.html");
	headers["Content-Length"] = "33";

}
void response::moved_permanently(request req){
	statusLine = "HTTP/1.1 301 Moved Permanently";
	headers["Location"] = req.getUrl() + "/";
	body = readFromFile("./Pages/301.html");
	headers["Content-Length"] = to_string(body.size());

}
response::ultimate response::info(std::string res_path){
	ultimate res;
	struct stat st;
	res.badtrip = (stat(res_path.c_str(), &st) != 0);
	res.is_file = S_ISREG(st.st_mode);
	res.is_dir = S_ISDIR(st.st_mode);
	res.read_per = (st.st_mode & S_IRUSR) != 0;
	res.write_per = (st.st_mode & S_IWUSR) != 0;
	res.exec_per = (st.st_mode & S_IXUSR) != 0;
	return res;
}

bool response::get_resources(request & req,location loc)
{
	auto_index = !loc.getDirective("autoIndex").compare("on") ;
	path = loc.getDirective("root") + removeLoc(req);
	int q_pos = path.find("?");
	if (q_pos != static_cast<int>(std::string::npos)){
			query_string = path.substr(q_pos + 1, path.size());
			path = path.substr(0, q_pos);
		}
	// struct stat st; 
	mode = info(path); 
	if(mode.badtrip)
		return false;
return true;
}

bool response::handel_get(request &req, location loc){
	if(!get_resources(req,loc)){
		notFound(req);
		return true;
	}
	else
	{
		if(!mode.read_per)
			Forbidden();
		if(mode.is_dir)
		{
			if(path.at(path.size() - 1) == '/'){		
				std::string line ;
				std::string tmp = path + "/index.html";
				std::ifstream file_(tmp.c_str());
				if(info(path + "/index.html").is_file){
					std::ostringstream file_content;
					statusLine = "HTTP/1.1 200 OK";
					while(std::getline(file_,line)){
						body += line + '\n';
						// std::cout << line << std::endl;
					}
				}
				else if(auto_index)
					list_directory(path);
				else
					Forbidden();
				headers["Content-Length"] = to_string(body.size());
				return(true);
			}
			else
				moved_permanently(req);
				// headers["Content-Length"] = to_string(body.size());
			
		}
		else if(mode.is_file){
			if (locationHasCgi(path)){
				cgiGet(path, query_string);
			}
			else{
				statusLine = "HTTP/1.1 200 OK";
				std::ifstream file_(path.c_str());
				std::string line;
				while(std::getline(file_,line)){
					body += line + '\n';
					// std::cout << line << std::endl;
				}
				headers["Content-Length"] = to_string(body.size());
			}
		}
		return(true);
	}
	
}
bool response::delete_directory(const std::string &path) {
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

void response::perrmission_denied(){
	statusLine = "HTTP/1.1 500 Internal Server Error";
	body = "perrmission denied";
	headers["Content-Length"] = to_string(body.size());
}
void response::Conflict()
{
	statusLine = "HTTP/1.1 500 Internal Server Error";
	body = "perrmission denied";
	headers["Content-Length"] = to_string(body.size());

}
bool response::handle_delete(request &req , location &loc)
{


   if(!get_resources(req,loc))
		notFound(req);
    else if(mode.is_dir)//mode_t st_mode: File mode, which includes the file type and file mode bits (permissions).
    {
		if(path.at(path.size() - 1) == '/'){
        // hna khasni nchecky permissions

            if (delete_directory(path)) {
				std::cout << "path = "<< "\'" << path << "\'"  << std::endl;
                statusLine = "HTTP/1.1 200 OK ";
                body = "Directory deleted";
                headers["Content-Length"] = to_string(body.size());
            } else
			{
				if(mode.write_per)
					perrmission_denied();
				else
					Forbidden();
					
			}
		}
		else
			Conflict();
		
	}else
    {

		
        if(mode.write_per){
            std::remove(path.c_str());
            statusLine = "HTTP/1.1 200 OK ";
            body = "File deleted";
            headers["Content-Length"] = to_string(body.size());
        }
        else
			perrmission_denied();

    }
    return true;
	}


		
void response::generate(request &req, ServerData &serv){
	if (isReqWellFormated(req, serv)){
		if (isLocation(req, serv)){
			location loc = getLoc(req, serv);
			if (!isLocationHaveRedi(loc)){
				if (isMethodAllowed(req, loc)){
					if (req.getMethod() == request::POST)
						handlePost(req, loc);
					else if (req.getMethod() ==request::GET)
						handel_get(req, loc);
					else if(req.getMethod() == request::DELETE)
						handle_delete(req,loc);
				}
			}
		}
	}
}
void response::sending(int cltFd){
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

void response::responce_(response::STATUS_CODE status)
{
	if(status == 0)
		return;

}