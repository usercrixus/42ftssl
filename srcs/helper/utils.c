#include "utils.h"
#include "../../42libft/ft_str/ft_str.h"
#include <stdio.h>

unsigned char *read_all_stdin(size_t *out_len)
{
	t_str *buff = ft_str_create();
	if (!buff)
	{
		perror("malloc");
		exit(1);
	}
	char tmp[4096];
	ssize_t len;
	while ((len = read(STDIN_FILENO, tmp, sizeof(tmp))) > 0)
		ft_str_push(buff, tmp, len);
	if (len < 0)
	{
		perror("read");
		ft_str_free(buff);
		exit(1);
	}
	char *result = ft_str_get_char_array(buff, buff->size);
	*out_len = buff->size;
	return (ft_str_free(buff), (unsigned char *)result);
}

void print_hex(const unsigned char *d, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        printf("%02x", d[i]);
}