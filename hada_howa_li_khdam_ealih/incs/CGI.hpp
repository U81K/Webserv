#include"webserv.hpp"

class cgi{
    private:
    int fd[2];
    char **env;
    void execute();
    public:
    cgi(){}
    void common_geteway_interface();



};