NAME					=	webserv

SRC						=	Cgi.cpp LocationConfig.cpp \
							main.cpp ConfigParsing.cpp Request.cpp \
							Response.cpp ResponseHeader.cpp \
							ServerConfig.cpp utils.cpp Webserv.cpp \

GREEN					=	\033[0;32m
RED						=	\033[0;31m
NO_COLOR				=	\033[0m

OBJ 					= 	$(SRC:.cpp=.o)
OBJ_DIR					=	obj
OBJ_FILES				=	$(addprefix $(OBJ_DIR)/, $(OBJ))
UPLOADS_DIR				=	uploads
UPLOADS					=	www/upload

CFLAGS					=	-g -fsanitize=address -Werror -Wextra -Wall -std=c++98

$(OBJ_DIR)/%.o			: 	src/%.cpp
							@mkdir -p $(OBJ_DIR)
							@c++ $(CFLAGS) -c $< -o $@

all						:	$(NAME)
							@echo "$(GREEN)Starting webserver...$(NO_COLOR)"

$(NAME)					:	$(OBJ_FILES)
							@mkdir -p $(OBJ_DIR)
							@c++ $(CFLAGS) -o $(NAME) $(OBJ_FILES)

clean					:
							@rm -rf $(OBJ_DIR) $(UPLOADS_DIR) $(UPLOADS)

fclean					:	clean
							@rm -rf $(NAME)
							@echo "$(RED)Webserver stopped$(NO_COLOR)"
							@killall $(NAME) 2>/dev/null || true

re						:	fclean all

.PHONY					:	all clean fclean re