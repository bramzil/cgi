###################### variables ######################

SRCS		=	main.cpp core/Core.cpp \
				core/Server.cpp \
				core/Location.cpp \
				core/Socket.cpp \
				core/Event.cpp \
				core/Connection.cpp \
				core/ServerSocket.cpp \
				request/Request.cpp \
				request/CgiModule.cpp \
				request/RequestLine.cpp \
				request/RequestProcessor.cpp \
				request/MultipartParser.cpp \
				response/Response.cpp \
				parser/HttpParser.cpp \
				parser/ServParser.cpp \
				parser/LocParser.cpp \
				parser/ParserExcep.cpp \
				utilities.cpp

HEADERS		=	includes/core/Core.hpp \
				includes/core/Server.hpp \
				includes/core/Location.hpp \
				includes/core/Socket.hpp \
				includes/core/Event.hpp \
				includes/core/Connection.hpp \
				includes/core/ServerSocket.hpp \
				includes/request/Request.hpp \
				includes/request/CgiModule.hpp \
				includes/request/RequestLine.hpp \
				includes/request/RequestProcessor.hpp \
				includes/request/MultipartParser.hpp \
				includes/response/Response.hpp \
				includes/parser/HttpParser.hpp \
				includes/parser/ServParser.hpp \
				includes/parser/LocParser.hpp \
				includes/parser/ParserExcep.hpp \
				includes/utilities.hpp

OBJS		=	$(SRCS:%.cpp=%.o)

NAME		=	Server

FLAGS		=	-Wall -Wextra -Werror -std=c++98 -g -fsanitize=address

######################### rules #######################

all			: $(NAME)

%.o 		: %.cpp $(HEADERS)
	g++ -c $(FLAGS) $< -o $@

$(NAME)		: $(OBJS) $(HEADERS)
	g++ $(FLAGS) $(OBJS) -o $(NAME)

clean		:
	rm -rf $(OBJS)

fclean		: clean
	rm -rf $(NAME)

re 			: fclean all