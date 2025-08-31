#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "helper/parse.h"
#include "md5/md5.h"
#include "sha256/sha256.h"
#include "../42libft/ft_printf/ft_printf.h"

typedef void (*dispatch_fn)(char *flags, char *input);

/* return FLAGS without any char present in DROP (static buffer) */
static char *flags_without_many(const char *flags, const char *drop)
{
	static char buff[64];
	size_t buff_len = 0;
	if (!flags)
		return NULL;
	for (size_t i = 0; flags[i] && buff_len + 1 < sizeof(buff); ++i)
		if (!strchr(drop, flags[i]))
			buff[buff_len++] = flags[i];
	buff[buff_len] = '\0';
	return (buff_len ? buff : NULL);
}

/* perform dispatch for md5 (keeps your original MD5-only fallback) */
static void dispatch(const t_cmd_data *cli, dispatch_fn fn)
{
	if (cli == NULL)
		fn(NULL, NULL);
	else
	{
		if (isFlagSet(cli->flags, 'p'))
			fn(cli->flags, NULL);
		if (isFlagSet(cli->flags, 's') && cli->s_arg)
			fn(cli->flags, cli->s_arg);
		if (cli->file_arg)
			fn(flags_without_many(cli->flags, "sp"), cli->file_arg);
		if (!isFlagSet(cli->flags, 'p') && !(isFlagSet(cli->flags, 's') && cli->s_arg) && !cli->file_arg && !isatty(STDIN_FILENO))
			fn(cli->flags, NULL);
	}
}

static dispatch_fn get_cmd(const char *name)
{
	static const char *FNAME[] = {"md5", "sha256"};
	static const dispatch_fn DISPATCH[] = {manageMD5, manageSHA256};
	for (int i = 0; i < (int)(sizeof(FNAME) / sizeof(FNAME[0])); ++i)
		if (ft_strncmp(name, FNAME[i], ft_strlen(FNAME[i]) + 1) == 0)
			return DISPATCH[i];
	return NULL;
}

static void print_errors(char *name, const t_cmd_data *cli)
{
	for (int i = 0; i < cli->errc; ++i)
		ft_printf("ft_ssl: %s: %s: No such file or directory\n", name, cli->errv[i]);
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return (ft_printf("usage: ft_ssl command [flags] [file/string]\n"), 1);
	dispatch_fn  fn = get_cmd(argv[1]);
	if (fn == NULL)
		return (ft_printf("ft_ssl: Error: '%s' is an invalid command\n", argv[1]), 1);
	if (argc < 3)
	{
		if (!isatty(STDIN_FILENO))
		{
			dispatch(NULL, fn);
			return 0;
		}
		return (ft_printf("usage: ft_ssl command [flags] [file/string]\n"), 1);
	}
	t_cmd_data data;
	parse_data(argc, argv, &data);
	dispatch(&data, fn);
	print_errors(argv[1], &data);
	return 0;
}
