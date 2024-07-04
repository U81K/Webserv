/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiTesting.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 14:09:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/07/04 11:49:15 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <vector>

char** env_to_char_array(std::map<std::string, std::string> env_vars) {
	std::size_t env_size = env_vars.size();
	char** env_array = new char*[env_size + 1];
	std::size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it = env_vars.begin(); it != env_vars.end(); ++i, it++) {
		std::string env_str = it->first + "=" + it->second;
		std::size_t str_len = env_str.length() + 1;
		env_array[i] = new char[str_len];
		strcpy(env_array[i], env_str.c_str());
	}
	env_array[env_size] = nullptr;

	return env_array;
}

// Function to count the number of elements in a null-terminated char** array
size_t countEnv(char** env) {
    size_t count = 0;
    while (env[count] != nullptr) {
        count++;
    }
    return count;
}

// Function to append new environment variables
char** appendEnv(char** envPtr, std::string QueryStr) {
    // Define new environment variables
    std::vector<std::string> newEnvVars;
	newEnvVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	newEnvVars.push_back("REQUEST_METHOD=GET");
	newEnvVars.push_back("SCRIPT_FILENAME=/path/to/your/script.php");
	newEnvVars.push_back(QueryStr);

    // Count the number of existing environment variables
    size_t existingCount = countEnv(envPtr);

    // Allocate a new array to hold the existing and new environment variables
    char** newEnvPtr = new char*[existingCount + newEnvVars.size() + 1];

    // Copy existing environment variables to the new array
    for (size_t i = 0; i < existingCount; ++i) {
        newEnvPtr[i] = envPtr[i];
    }

    // Add new environment variables to the new array
    for (size_t i = 0; i < newEnvVars.size(); ++i) {
        newEnvPtr[existingCount + i] = new char[newEnvVars[i].size() + 1];
        std::strcpy(newEnvPtr[existingCount + i], newEnvVars[i].c_str());
    }

    // Null-terminate the new array
    newEnvPtr[existingCount + newEnvVars.size()] = nullptr;

    return newEnvPtr;
}

int cgiExcution(std::string scriptPath, request &req){
	std::ifstream scriptFile(scriptPath);
	if (!scriptFile.is_open()){
		std::cerr << "error opening the script file\n";
		return (1);
	}
	// scriptPath = "python " + scriptPath;
	int pipefd[2];
	if (pipe(pipefd) == -1){
		std::cerr << "error piping\n";
		return (1);
	}
	pid_t pid = fork();
	if (pid == 0){
		close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[1]);
		char **fullEnv = appendEnv(envPtr, query);
		// for (int i = 0; fullEnv[i]; i++){
		// 	std::cout << fullEnv[i] << std::endl;
		// }
		std::string pyPath = "/usr/bin/python";
		char* args[] = {(char *)pyPath.c_str(), (char *)scriptPath.c_str(), nullptr};
		if (execve("/usr/bin/python", args, fullEnv) < 0){
			std::cerr << "error execve\n";
			return (-1);
		}
		exit(1);
	}
	else{
		// Parent process
        close(pipefd[1]); // Close write end
		int status;
		waitpid(pid, &status, 0);
		// Read output from the pipe
        char buffer[4096];
        ssize_t bytesRead;
		std::string body;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
			std::string buf(buffer, bytesRead);
			body = body.append(buf);
        }
        close(pipefd[0]);

		if (WIFEXITED(status)){
			if (WEXITSTATUS(status) == 0) {
                // CGI program exited successfully
                // ... (handle successful execution)
				std::cout << body ;
                return (0);
            } else {
                std::cerr << "CGI program exited with error: " << WEXITSTATUS(status) << std::endl;
                return (-1);
            }
		}
		else
			return (-1);
	}
}

std::string getVarName(char *str){
	std::string ret(str);

	size_t pos = ret.find("=");
	if (pos != std::string::npos){
		ret = ret.substr(0, pos);
	}
	return (ret);
}

std::string getVarValue(char *str){
	std::string ret(str);
	size_t pos = ret.find("=");
	if (pos != std::string::npos){
		ret = ret.substr(pos + 1, ret.size());
	}
	return (ret);
}

std::map<std::string, std::string> getEnv(char **env){
	std::map<std::string, std::string> envMap;
	
	for (int i = 0; env[i]; i++){
		std::string name = getVarName(env[i]);
		std::string value = getVarValue(env[i]);
		envMap[name] = value;
	}
	
	return (envMap);
}

int main(int ac, char **av, char **env){
	if (ac != 2)
		return (1);
	std::string path = av[1];
	// std::cout << path << std::endl;
	// std::map<std::string, std::string> mapp = getEnv(env);
	
	cgiExcution(path, env, "QUERY_STRING=param1=value1&param2=value2");

	
	// for (std::map<std::string, std::string>::iterator it = mapp.begin(); it != mapp.end(); it++){
	// 	std::cout << it->first << "  " << it->second << std::endl;
	// }
	
	return (0);
}