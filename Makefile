NAME		:= webserv
CXX			:= clang++
CXXPPFLAGS	:= -I./includes
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -g

FILES	:= main Server Location HttpContext Header Event Config
RM		:= rm -rf
SRCS	:= $(addsuffix .cpp, $(addprefix srcs/, $(FILES)))
OBJS	:= $(SRCS:.cpp=.o)

.Phony: all re clean fclean

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CXXPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all