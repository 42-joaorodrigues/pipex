NAME	= pipex
BNAME	= pipex_bonus

LIBFT	= libft/free_split.c libft/ft_split.c libft/str_util.c \
		  libft/gnl.c libft/gnl_util.c

SRC		= src/mandatory/pipex.c src/exec.c
OBJ		= $(SRC:%.c=obj/%.o) $(LIBFT:%.c=obj/%.o)

BSRC	= src/bonus/pipex_bonus.c src/bonus/util_bonus.c src/exec.c
BOBJ	= $(BSRC:%.c=obj/%.o) $(LIBFT:%.c=obj/%.o)

all: $(NAME)

bonus: $(BNAME)

obj/%.o: %.c
	mkdir -p $(dir $@)
	cc -Wall -Wextra -Werror -c $< -o $@ -I ./include

$(NAME): $(OBJ)
	cc $^ -o $@

$(BNAME): $(BOBJ)
	cc $^ -o $@

clean:
	rm -rf obj

fclean: clean
	rm -rf $(NAME)
	rm -rf $(BNAME)

re: fclean all
