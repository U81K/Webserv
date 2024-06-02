#include "../incs/get.hpp"
#include <fstream>
#include <sstream>

// Get::Get(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations)
//     : method_(path, method, locations) {}

void Get::handle(const request& req, std::string& response) {
    auto it = locations.find(path);
    if (it == locations.end()) {
        response = "404 Not Found";
        return;
    }

    const ConfigLocation& configLocation = it->second;
    std::string filePath = configLocation.root + req.get_path();

    std::ifstream file(filePath);
    if (!file.is_open()) {
        response = "404 Not Found";
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    response = "200 OK\n\n" + buffer.str();
}
