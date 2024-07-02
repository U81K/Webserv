/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 19:03:42 by bgannoun          #+#    #+#             */
/*   Updated: 2024/05/19 19:06:01 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/response.hpp"

bool responce::foundInAllowedChat(char c)
{
    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        return (true);
    std::string str = ":/?#[]@!$&'()*+,;=";
    if (str.find(c) != std::string::npos)
        return (true);
    return (false);
}

std::string responce::readFromFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()){
        std::cerr << "unable to open the file: " << filePath << std::endl;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return (ss.str());
}

void responce::clear(){
    statusLine.clear();
    headers.clear();
    body.clear();
}

void responce::statusBadRequest(request &req){
    statusLine = req.getHttpV() + " 400 Bad Request";
    body = readFromFile("./pages/400.html");
    headers["Content-Length"] = std::to_string(body.size());
}

void responce::statusrequestUriTooLarge(request &req){
    statusLine = req.getHttpV() + " 414 request-uri too large";
    body = readFromFile("./pages/414.html");
    headers["Content-Length"] = std::to_string(body.size());
}

void responce::requestEntityTooLarge(request &req){
    statusLine = req.getHttpV() + " 403 request entity too large";
    body = readFromFile("./pages/403.html");
    headers["Content-Length"] = std::to_string(body.size());
}

void responce::notImplemented(request &req){
    statusLine = req.getHttpV() + " 501 not implemented";
    body = readFromFile("./pages/501.html");
    headers["Content-Length"] = std::to_string(body.size());
}

bool responce::isReqWellFormated(request &req, ServerData &server)
{
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

void responce::notFound(request &req){
    statusLine = req.getHttpV() + " 404 not found";
    body = readFromFile("./pages/404.html");
    headers["Content-Length"] = std::to_string(body.size());
}

std::string responce::getLocation(const std::string &uri){
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

bool responce::isLocation(request &req, ServerData &server){
    (void) req;
    (void) server;
    // std::string url = getLocation(req.getUrl());
    // // std::cout << url << std::endl;
    // std::vector<location> locs = server.getLocs();
    // for(unsigned int i = 0; i < locs.size(); i++){
    // 	if (url.compare(locs[i].getPath()) == 0){
            return (true);
    // 	}
    // }
    // notFound(req);
    // return (false);
}

location responce::getLoc(request &req, ServerData &server){
    std::string url = getLocation(req.getUrl());
    std::vector<location> locs = server.getLocs();
    for(unsigned int i = 0; i < locs.size(); i++){
        if (url.compare(locs[i].getPath()) == 0){
            return (locs[i]);
        }
    }
    return (location());
}
bool responce::isMethodAllowed(request &req, location loc){
    (void) req;
    (void) loc;
    // std::string method;
    // if (req.getMethod() == 0)
    // 	method = "GET";
    // else if (req.getMethod() == 1)
    // 	method = "POST";
    // else if (req.getMethod() == 2)
    // 	method = "DELETE";
    // else if (req.getMethod() == 3)
    // 	method = "UNKNOWN";
    // std::string allowedMethods = loc.getDirective("acceptedMethods");
    // if ((allowedMethods.find(method) != std::string::npos)){
        return (true);
    // }
    // statusLine = "HTTP/1.1 405 Method Not Allowed";
    // body = "method not allowed";
    // headers["Content-Length"] = "18";
    // return (false);
}
std::string responce::generateListItems(const std::string &fileAndDirNames) 
{
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
bool responce::delete_directory(const std::string &path) {
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

bool responce::handle_delete(request &req, ServerData &serv)
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
        if(object_stat.st_mode & S_IWUSR){

            if (delete_directory(path)) {
                statusLine = "HTTP/1.1 200 OK ";
                body = "Directory deleted";
                headers["Content-Length"] = std::to_string(body.size());
            } else {
                statusLine = "HTTP/1.1 500 Internal Server Error";
                body = "bad trip!";
                headers["Content-Length"] = std::to_string(body.size());
            }
        }
        else {
            statusLine = "HTTP/1.1 500 Internal Server Error";
            body = "perrmission denied";
            headers["Content-Length"] = std::to_string(body.size());
        }
    }
    else
    {
        if(object_stat.st_mode & S_IWUSR){
            std::remove(path.c_str());
            statusLine = "HTTP/1.1 200 OK ";
            body = "File deleted";
            headers["Content-Length"] = std::to_string(body.size());
        }
        else {
            statusLine = "HTTP/1.1 500 Internal Server Error";
            body = "perrmission denied";
            headers["Content-Length"] = std::to_string(body.size());
        }

    }
    return true;
}

bool responce::list_directory(std::string &dir_path) {

    statusLine = "HTTP/1.1 200 OK";
    // body += "Index of " + dir_path + "\n\r";
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
    std::cout << body << std::endl;
    // std::string size = "" + body.size();
    std::cout << "body = " << body << std::endl;
    headers["Content-Length"] = std::to_string(body.size());
    return true;
}
bool responce::isLocationHaveRedi(location loc){
    std::string dir = loc.getDirective("return");
    if (dir.size() > 0){
        statusLine = "HTTP/1.1 301 Moved Permanently";
        headers["Location"] = dir;
        return (true);
    }
    return (false);
}
bool responce::get_resources(request & req)
{
    root = "www";
    is_dir = false;
    auto_index = true;
    path = "./" + root + req.getUrl();
    // (void) req;
// 	//  ia makanch l file fe root err 
// 	// std::string root = "";
// 	root = "/www";
// 	path = "." + root +req.getUrl();
// 	std::cout << "\033[36m";
// 	std::cout << "root path " << path << std::endl;
// 	// std::string path = "." + req.getUrl();
// 	struct stat object_stat;
// 	// object_stat.
// 	if(stat(path.c_str(),&object_stat) != 0)
// 		return false;
    return true;
}
bool responce::handel_get(request &req, ServerData &serv){
    (void) serv;
    if(!get_resources(req))
    {
        notFound(req);
        return false;
    }
    else
    {
        // std::cout << "\n\n\n\n"<< "bad trip" << req.getUrl() << "\n\n\n\n\n\n\n";
        std::string line ;
        std::ifstream file_("www/index.html");
        if(auto_index){
            std::ostringstream file_content;
            statusLine = "HTTP/1.1 200 OK";
            // file_content << "www/index.html";
            while(std::getline(file_,line))
            {
                body += line + '\n';
                std::cout << line << std::endl;

            }
            headers["Content-Length"] = std::to_string(body.size());
            return(true);
        }
        else if(is_dir)
            list_directory(path);
        else {
                std::ifstream  zeb(path);
                std::string line ;
                statusLine = "HTTP/1.1 200 OK";
                while(getline(zeb,line))
                    body += line + "\n";
        }
        headers["Content-Length"] = std::to_string(body.size());    
        return(true);
    }
}
void responce::generate(request &req, ServerData &serv){
    if (isReqWellFormated(req, serv)){
        if (isLocation(req, serv)){
            location loc = getLoc(req, serv);
            if (!isLocationHaveRedi(loc)){
                if (isMethodAllowed(req, loc)){
                    // statusLine = "HTTP/1.1 200 OK";
                    // body = "hello world!";
                    // headers["Content-Length"] = "12";
                    if(req.getMethod() == request::DELETE)
                        handle_delete(req,serv);
                    else if(req.getMethod() == request::GET){
                        handel_get(req,serv);
                    // else if(req.getMethod() == request::POST)
                    // 	handel_post();
                        // exit(0);
                    }
                }
            }
        }
    }
}
void responce::sending(int cltFd){
    std::ostringstream response;
    response << statusLine << "\r\n";
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << body;
    std::cout << "full response = " << "\033[1;31m"<< response.str() <<  std::endl;
    std::string res = response.str();
    send(cltFd, res.c_str(), res.size(), 0);
}












