NAME	= pipex
LIBFT	= libft/free_split.c libft/ft_split.c libft/str_util.c
SRC		= src/pipex.c src/exec.c
OBJ		= $(SRC:%.c=obj/%.o) $(LIBFT:%.c=obj/%.o)

all: $(NAME)

obj/%.o: %.c
	mkdir -p $(dir $@)
	cc -Wall -Wextra -Werror -c $< -o $@ -I ./include

$(NAME): $(OBJ)
	cc $^ -o $@

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)

re: fclean all
