NAME					=	webserv

SRC_DIR					=	src
SRC						=	$(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)

OBJ_DIR					=	obj
OBJ						=	$(patsubst $(SRC_DIR)/%.cpp,%.o,$(SRC))
OBJ_FILES				=	$(addprefix $(OBJ_DIR)/, $(OBJ))

CFLAGS					=	-Werror -Wextra -Wall -std=c++98

RED						=	\033[0;31m
GREEN					=	\033[0;32m
NC						=	\033[0m

$(OBJ_DIR)/%.o			: 	$(SRC_DIR)/%.cpp
							@mkdir -p $(dir $@)
							@c++ $(CFLAGS) -c $< -o $@

all						:	$(NAME)

$(NAME)					:	$(OBJ_FILES)
							@mkdir -p $(OBJ_DIR)
							@c++ $(CFLAGS) -o $(NAME) $(OBJ_FILES)
							@echo "$(GREEN)Starting Webserver...$(NC)"

clean					:
							@echo "$(RED)Cleaning object files...$(NC)"
							@rm -rf $(OBJ_DIR)

fclean					:	clean
							@rm -rf $(NAME)

re						:	fclean all

.PHONY					:	all clean fclean re
