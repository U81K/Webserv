/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readFromFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 14:09:06 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/14 14:17:32 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>

std::string readFromFile(const std::string &filePath){
	std::ifstream file(filePath);
	if (!file.is_open()){
		std::cerr << "unable to open the file: " << filePath << std::endl;
		return ("");
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}

int main(void){
	std::cout << readFromFile("/") << std::endl;
	return (0);
}