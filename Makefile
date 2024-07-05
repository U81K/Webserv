# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/17 14:25:00 by bgannoun          #+#    #+#              #
#    Updated: 2024/06/16 15:22:42 by bgannoun         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv
RM			=	rm -rf
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
SRCS		=	srcs/main.cpp srcs/ServerData.cpp srcs/ClientData.cpp srcs/request.cpp srcs/response.cpp
OBJS		=	$(SRCS:.cpp=.o)
INCS		=	incs/webserv.hpp incs/ClientData.hpp incs/request.hpp incs/ServerData.hpp incs/response.hpp

all			:	$(NAME)

$(NAME)		:	$(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean		:
	$(RM) $(OBJS)

fclean 		: clean
	$(RM) $(NAME)

re			:	fclean all
