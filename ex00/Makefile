# **************************************************************************** #
#                                                                              #
#    Makefile - ex00 (whatever)                                                #
#                                                                              #
# **************************************************************************** #

NAME		= whatever

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

SRCS		= main.cpp
OBJS		= $(SRCS:.cpp=.o)

# テンプレートの定義を含むヘッダ。これが変わったら再コンパイルする。
HEADERS		= whatever.hpp

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# .o は対応する .cpp とヘッダに依存させる（ヘッダ変更で再ビルド）。
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
