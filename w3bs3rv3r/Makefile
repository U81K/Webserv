# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/17 14:25:00 by bgannoun          #+#    #+#              #
#    Updated: 2024/05/17 19:18:39 by bgannoun         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv
RM			=	rm -rf
CXXFLAGS	=	#-Wall -Wextra -Werror -std=c++98
SRCS		=	srcs/main.cpp srcs/ServerData.cpp srcs/ClientData.cpp srcs/request.cpp srcs/responce.cpp srcs/config.cpp 
OBJS		=	$(SRCS:.cpp=.o)
INCS		=	incs/webserv.hpp incs/ClientData.hpp incs/request.hpp incs/ServerData.hpp

all			:	$(NAME)

$(NAME)		:	$(OBJS) $(INCS)
	c++ $(CXXFLAGS) $(OBJS) -o $(NAME)

clean		:
	$(RM) $(OBJS)

fclean 		: clean
	$(RM) $(NAME)

re			:	fclean all