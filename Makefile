NAME		:= webserv
CXX			:= g++
CXXPPFLAGS	:= -I./includes
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -g

FILES	:=	parser/Parser parser/Server parser/Location parser/Pages \
			parser/Server_directives parser/Location_directives \
			main HttpContext Multiplexer Request Client Response Connection

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
