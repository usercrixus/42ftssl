#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "helper/parse.h"
#include "md5/md5.h"
#include "sha256/sha256.h"

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
static void dispatch_md5(const t_cmd_data *cli)
{
	if (isFlagSet(cli->flags, 'p'))
		manageMD5(cli->flags, NULL);
	if (isFlagSet(cli->flags, 's') && cli->s_arg)
		manageMD5(cli->flags, cli->s_arg);
	if (cli->file_arg)
		manageMD5(flags_without_many(cli->flags, "sp"), cli->file_arg);
	if (!isFlagSet(cli->flags, 'p') && !(isFlagSet(cli->flags, 's') && cli->s_arg) && !cli->file_arg && !isatty(STDIN_FILENO))
		manageMD5(cli->flags, NULL);
}

/* perform dispatch for sha256 (no final fallback; mirrors your current behavior) */
static void dispatch_sha256(const t_cmd_data *cli)
{
	if (isFlagSet(cli->flags, 'p'))
		manageSHA256(cli->flags, NULL);
	if (isFlagSet(cli->flags, 's') && cli->s_arg)
		manageSHA256(cli->flags, cli->s_arg);
	if (cli->file_arg)
		manageSHA256(flags_without_many(cli->flags, "sp"), cli->file_arg);
}

/* print queued CLI errors (extra args) */
static void print_errors(t_option option, const t_cmd_data *cli)
{
	const char *name = (option == MD5) ? "md5" : "sha256";
	for (int i = 0; i < cli->errc; ++i)
		fprintf(stderr, "ft_ssl: %s: %s: No such file or directory\n", name, cli->errv[i]);
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);
	t_option option = parse_cmd(argv[1]);
	if (option == NONE)
		return (fprintf(stderr, "ft_ssl: Error: '%s' is an invalid command", argv[1]), 1);
	/* command only; if stdin is piped, hash it with default formatting */
	if (argc < 3)
	{
		if (!isatty(STDIN_FILENO))
		{
			if (option == MD5)
				manageMD5(NULL, NULL);
			else
				manageSHA256(NULL, NULL);
			return 0;
		}
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);
	}
	t_cmd_data data;
	parse_data(argc, argv, &data);
	if (option == MD5)
		dispatch_md5(&data);
	else
		dispatch_sha256(&data);
	fflush(stdout);
	print_errors(option, &data);
	return 0;
}
