/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 13:03:21 by bgannoun          #+#    #+#             */
/*   Updated: 2024/04/28 23:33:20 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientdata_hpp
#define clientdata_hpp

#include <iostream>

class ClientData{
	private:
		int sockfd;
		struct sockaddr_in addr;
	public:
		ClientData(){}
		ClientData(int fd, struct sockaddr_in a) : sockfd(fd), addr(a) {
		}
		int getSocketFd(){
			return (sockfd);
		}
		// std::string getIp(){
		// 	return (ip);
		// }
		// int getPort(){
		// 	return (port);
		// }
};

#endif