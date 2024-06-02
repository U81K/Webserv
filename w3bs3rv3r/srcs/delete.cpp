#include "../incs/delete.hpp"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Delete::Delete(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations)
    : method_(path, method, locations) {}

void Delete::handle(const request& req, std::string& response) {
    auto it = locations.find(path);
    if (it == locations.end()) 
    {
        response = "404 Not Found";
        return;
    }

    const ConfigLocation& configLocation = it->second;
    std::string filePath = configLocation.root + req.get_path();

    if (!fs::exists(filePath)) {
        response = "404 Not Found";
        return;
    }

    try {
        fs::remove(filePath);
        response = "204 No Content";
    } catch (const fs::filesystem_error& e) {
        response = "500 Internal Server Error";
    }
}
