#pragma once

#include "method.hpp"
#include "request.hpp"

class Delete: public method_ {
public:
    // Delete(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations);
    void handle(const request& req, std::string& response) override;
};
