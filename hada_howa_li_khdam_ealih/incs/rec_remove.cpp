#include <iostream>
#include <string>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h> // for rmdir
#include <cstring>

// Function to delete a directory recursively
bool delete_directory(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << path << std::endl;
        return false;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            std::string full_path = path + "/" + entry->d_name;
            struct stat st;
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

// Function to handle delete request
bool handle_delete(request &req, ServerData &serv)
 {
    std::string path = "." + req.getUrl();
    if (delete_directory(path)) {
        std::remove(path.c_str());
        statusLine = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\n bad trip!";
    } else {
        statusLine = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\nConnection: close\r\n\r\n Failed to delete dir!";
    }
    return true;
}
