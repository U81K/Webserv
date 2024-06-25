/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiTesting.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 14:09:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/25 12:59:10 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int cgiExcution(std::string scriptPath){
	pid_t pid = fork();
	if (pid == 0){
		char* args[] = {(char *)scriptPath.c_str(), nullptr};
		if (execve(scriptPath.c_str(), args, NULL) < 0){
			std::cerr << "error execve\n";
			return (-1);
		}
		exit(1);
	}
	else{
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)){
			if (WEXITSTATUS(status) == 0) {
                // CGI program exited successfully
                // ... (handle successful execution)
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

int main(int ac, char **av, char **env){
	if (ac != 2)
		return (1);
	std::string path = av[1];
	std::cout << path << std::endl;
	cgiExcution(path);
	return (0);
}