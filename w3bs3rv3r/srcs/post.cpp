#include "../incs/post.hpp"
#include <fstream>
#include <sstream>

// Post::Post(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations)
//     : method_(path, method, locations) {}

void Post::handle(const request& req, std::string& response) {
    auto it = locations.find(path);
    if (it == locations.end()) {
        response = "404 Not Found";
        return;
    }

    const ConfigLocation& configLocation = it->second;
    std::string uploadPath = configLocation.upload_path + "/uploaded_file";

    std::ofstream file(uploadPath);
    if (!file.is_open()) {
        response = "500 Internal Server Error";
        return;
    }

    file << req.get_body();
    response = "201 Created";
}
