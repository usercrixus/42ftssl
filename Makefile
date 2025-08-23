OBJ = \
	srcs/openssl.o \
	srcs/md5/md5.o \
	srcs/md5/md5Encode.o \
	srcs/sha256/sha256.o \
	srcs/sha256/sha256Encode.o \
	srcs/helper/parse.o \
	srcs/helper/utils.o \

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

test:
	dd if=/dev/zero bs=1000000 count=1 2>/dev/null | tr '\0' 'A' | \
	valgrind --leak-check=full --show-leak-kinds=all ./ft_ssl md5

	dd if=/dev/zero bs=1000000 count=1 2>/dev/null | tr '\0' 'A' | \
	valgrind --leak-check=full --show-leak-kinds=all ./ft_ssl sha256

	./test/test_ft_ssl.sh 

.PHONY: clean fclean re test