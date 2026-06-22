# **************************************************************************** #
#                                                                              #
#    Makefile - ex02 (Array)                                                   #
#                                                                              #
# **************************************************************************** #

NAME		= array

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

SRCS		= main.cpp
OBJS		= $(SRCS:.cpp=.o)

# 宣言(Array.hpp)と実装(Array.tpp)の両方に依存させる。
HEADERS		= Array.hpp Array.tpp

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
