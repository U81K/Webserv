#pragma once

#include "method.hpp"
#include "request.hpp"

class Get : public method_ {
public:
    // Get(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations);
    void handle(const request& req, std::string& response) override;
};
