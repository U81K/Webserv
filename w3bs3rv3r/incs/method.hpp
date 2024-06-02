#include <iostream>
#include <map>
#include <vector>
#pragma once

class method_ {
protected:
    std::string path;
    std::string method;
    // const std::map<std::string, ConfigLocation>& locations;
public:
    // method_(const std::string& path, const std::string& method, const std::map<std::string, ConfigLocation>& locations)
        // : path(path), method(method), locations(locations) {}
    //  method_(){}
    virtual ~method_() {}

    virtual void handle(const request& req, std::string& response) = 0;

    std::string getPath() const { return path; }
    std::string getMethod() const { return method; }
};
