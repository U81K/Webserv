#pragma once
#include <string>
#include <iostream>
#include <map>
#include"request.hpp"
#include "config.hpp"
#include "method.hpp"
#include "get.hpp"
#include "post.hpp"
#include "delete.hpp"


class responce_ {
private:
    request *Request_;
    method_ *method;
    std::string responce;
    void process_request(void);
    void generate_html_responce(void);
    void process_method(void);
    void handle_request(void);        void handle_request(void);

public:
    // responce_(const request& request_, const std::map<std::string, ConfigLocation>& locations);
    // responce_(const std::string& method, const std::map<std::string, ConfigLocation>& locations);
        responce_(request& request_);
        responce_(){}
        responce_(std::string method);
        responce_(method_ &method);
        ~responce_();
        void set_responce(std::string responce);
        const char * c_str();
        size_t size();
        // void set_method(
};
