OBJ = \
	srcs/openssl.o \
	srcs/md5/md5.o \
	srcs/md5/md5Encode.o \
	srcs/sha256/sha256.o \
	srcs/sha256/sha256Encode.o \
	srcs/parse.o \

all: ft_ssl

ft_ssl: submodule libft.a $(OBJ)
	gcc $(OBJ) -L./42libft/ft_str -lftstr -o $@

%.o: %.c
	gcc -c -Wall -Wextra -Werror $< -o $@

submodule:
	git submodule update --init --recursive

libft.a:
	make -C ./42libft

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f ft_ssl

re: fclean all

.PHONY: clean fclean re