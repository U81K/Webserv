std::string  cgi::fill_env(std::string SCRIPT_FILENAME, std::string CGI)
{
     std::string response;
    int status;
    int fd[2];
    pid_t pid;
    int startTime = time(0);
    if (pipe(fd) == -1)
       return "";
    pid = fork();
    if (pid == -1)
        return "";
    if (pid == 0)
    {
        if (this->env["REQUEST_METHOD"] == "POST")
            dup2(fd[0], 0);
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        char **av = new char *[3];
        av[0] = new char[CGI.length() + 1];
        av[1] = new char[std::strlen(SCRIPT_FILENAME.c_str()) + 1];
        av[2] = NULL;
        std::strcpy(av[0], CGI.c_str());
        std::strcpy(av[1], SCRIPT_FILENAME.c_str());
        if (execve(CGI.c_str(), av, envp) == -1)
            exit(127);
    }
    else
    {
        if (this->env["REQUEST_METHOD"] == "POST")
            write(fd[1], body.c_str(), body.length());
        int result;
        while ((result = waitpid(pid, &status, WNOHANG)) == 0)
        {
            if (result == 0)
            {
                if ((time(0) - startTime) > 8)
                {
                    response = "HTTP/1.1 500 Internal Server Error";
                    kill(pid, SIGKILL);
                    close(fd[1]);
                    return response;
                }
                sleep(1);
            }
        }
        if (WEXITSTATUS(status) != 0)
            return "";
        close(fd[1]);
        char buffer[1024];
        while (true)
        {
            int byte = read(fd[0], buffer, 1023);
            if (byte <= 0)
                break;
            if (byte == 1023)
                buffer[byte] = '\0';
            else
                buffer[byte + 1] = '\0';
            if (byte >= 0)
            {
                buffer[byte] = '\0';
                response += buffer;
            }
        }
        return response;
    }
    return "";
}