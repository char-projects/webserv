NAME					=	webserv

SRC						=	main.cpp Webserv.cpp Request.cpp  Response.cpp utils.cpp
C_DIR					= 	src

OBJ 					= 	$(SRC:.cpp=.o)
OBJ_DIR					=	obj
OBJ_FILES				=	$(addprefix $(OBJ_DIR)/, $(OBJ))

CFLAGS					=	-g -fsanitize=address -fno-omit-frame-pointer -Werror -Wextra -Wall -std=c++98

$(OBJ_DIR)/%.o			: 	$(C_DIR)/%.cpp
							@mkdir -p $(OBJ_DIR)
							c++ $(CFLAGS) -c $< -o $@

all						:	$(NAME)

$(NAME)					:	$(OBJ_FILES)
							@mkdir -p $(OBJ_DIR)
							c++ $(CFLAGS) -o $(NAME) $(OBJ_FILES)

clean					:
							rm -rf $(OBJ_DIR)

fclean					:	clean
							rm -rf $(NAME)

re						:	fclean all

.PHONY					:	all clean fclean re
