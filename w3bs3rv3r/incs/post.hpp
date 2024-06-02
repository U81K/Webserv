#pragma once

#include "method.hpp"
#include "request.hpp"

class Post: public method_ {
public:
    // Post(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations);
    void handle(const request& req, std::string& response) override;
};
