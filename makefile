
CFLAGS = -Wall 
SRC = src/main.c # src/utils.c src/parse.c src/execute.c src/builtins.c
OBJ = $(SRC:.c=.o)
NAME = minishell
all: $(NAME)
$(NAME): $(OBJ)
	gcc $(CFLAGS) -o $(NAME) $(OBJ)
clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
