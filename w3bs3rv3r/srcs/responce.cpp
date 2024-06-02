#include"../incs/responce.hpp"

responce_::responce_(method_ &method)
{
    this->method = &method;
}

responce_::responce_( request& request_):Request_(request_){}
responce_::responce_(std::string method)
{
    switch (method)
    {
    case "GET":
        method = new Get;
        break;
    case "POST"
        method = new Post;
    case "DELETE"
        method = new Delete;
    default:
        std::cout << "bad trip" << std::endl;
        break;
    }

}

responce_::~responce_()
{
    std::cout << "free hna azbi" << std::endl;
    delete method;
}

const char *responce_::c_str()
{
    return(responce.c_str());
}

void responce_::set_responce(std::string responce)
{
    this->responce = responce;
}

size_t responce_::size()
{
    return(responce.size());
}
void responce_::process_method(void)
{
    switch (Request_.get_method()) 
    {
        case request::GET:
        {
            std::cout << "GET" << std::endl;
            responce_("GET");
            break;
        }
        case request::POST:
        {
            std::cout << "POST" << std::endl;
            responce_("POST");
            break;
        }
        case request::DELETE:
        {
            std::cout << "DELETE" << std::endl;
            responce_("DELETE");
            break;
        }
        default:
            std::cout << "BAD TRIP" << std::endl;
            break;
    }
}
void responce_::handle_request(void)
{
    process_method();

    
}

void responce_::generate_html_responce(void)
{

}

void responce_::process_request(void)
{

}