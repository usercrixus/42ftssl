OBJ = \
	srcs/openssl.o \
	srcs/md5/md5.o \
	srcs/md5/md5Encode.o \
	srcs/parse.o \

all: openssl

openssl: submodule libft.a $(OBJ)
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
	rm -f openssl

re: fclean all

.PHONY: clean fclean re