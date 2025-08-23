#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "openssl.h"
#include "parse.h"

/* ─────────────── types ─────────────── */

typedef struct s_cli
{
	char *flags;	/* unique chars in combined flag groups */
	char *s_arg;	/* first -s argument (if any) */
	char *file_arg; /* first non-flag argument after flags */
	char *errv[32]; /* leftover args to report as errors */
	int errc;		/* error count */
} t_cli;

/* ─────────────── helpers ─────────────── */

static t_option parse_cmd(const char *cmd)
{
	if (!cmd)
		return NONE;
	if (strcmp(cmd, "md5") == 0)
		return MD5;
	if (strcmp(cmd, "sha256") == 0)
		return SHA256;
	return NONE;
}

/* return FLAGS without any char present in DROP (static buffer) */
static const char *flags_without_many(const char *flags, const char *drop)
{
	static char buf[64];
	size_t k = 0;
	if (!flags)
		return NULL;
	for (size_t i = 0; flags[i] && k + 1 < sizeof(buf); ++i)
		if (!strchr(drop, flags[i]))
			buf[k++] = flags[i];
	buf[k] = '\0';
	return (k ? buf : NULL);
}

/* add unique flag chars from token (like "-pqs"), capture -s VALUE if present */
static void absorb_flag_group(const char *tok, char *flags_buf, char **s_arg,
							  int argc, char **argv, int *i)
{
	const char *p = tok + 1; /* skip '-' */
	while (*p)
	{
		if (!strchr(flags_buf, *p))
		{
			size_t L = strlen(flags_buf);
			if (L + 2 < 64)
			{
				flags_buf[L] = *p;
				flags_buf[L + 1] = '\0';
			}
		}
		if (*p == 's')
		{
			if (*s_arg == NULL && *i + 1 < argc && argv[*i + 1][0] != '-')
			{
				*s_arg = argv[*i + 1];
				(*i)++; /* skip value */
			}
		}
		p++;
	}
}

/* parse argv into t_cli (flags, -s value, file, and trailing errors) */
static void parse_cli(int argc, char **argv, t_cli *o)
{
	static char flags_buf[64]; /* lifetime must outlive this function */
	int i = 2;

	memset(o, 0, sizeof(*o));
	flags_buf[0] = '\0';

	if (argc > 2 && argv[2][0] == '-')
	{
		while (i < argc && argv[i][0] == '-')
		{
			absorb_flag_group(argv[i], flags_buf, &o->s_arg, argc, argv, &i);
			i++;
		}
		if (flags_buf[0])
			o->flags = flags_buf;
		if (i < argc && argv[i][0] != '-')
			o->file_arg = argv[i++];
		for (int j = i; j < argc && o->errc < (int)(sizeof(o->errv) / sizeof(o->errv[0])); ++j)
			o->errv[o->errc++] = argv[j];
	}
	else if (argc > 2)
	{
		o->file_arg = argv[2];
	}
}

/* perform dispatch for md5 (keeps your original MD5-only fallback) */
static void dispatch_md5(const t_cli *cli)
{
	if (isFlagSet(cli->flags, 'p'))
		manageMD5(cli->flags, NULL);
	if (isFlagSet(cli->flags, 's') && cli->s_arg)
		manageMD5(cli->flags, cli->s_arg);
	if (cli->file_arg)
	{
		const char *ff = flags_without_many(cli->flags, "sp");
		manageMD5((char *)ff, cli->file_arg);
	}
	if (!isFlagSet(cli->flags, 'p') && !(isFlagSet(cli->flags, 's') && cli->s_arg) && !cli->file_arg && !isatty(STDIN_FILENO))
	{
		/* MD5 special-case fallback: pure piped stdin even if args parsed */
		manageMD5(cli->flags, NULL);
	}
}

/* perform dispatch for sha256 (no final fallback; mirrors your current behavior) */
static void dispatch_sha256(const t_cli *cli)
{
	if (isFlagSet(cli->flags, 'p'))
		manageSHA256(cli->flags, NULL);
	if (isFlagSet(cli->flags, 's') && cli->s_arg)
		manageSHA256(cli->flags, cli->s_arg);
	if (cli->file_arg)
	{
		const char *ff = flags_without_many(cli->flags, "sp");
		manageSHA256((char *)ff, cli->file_arg);
	}
}

/* print queued CLI errors (extra args) */
static void print_errors(t_option opt, char **argv, const t_cli *cli)
{
	const char *name = (opt == MD5) ? "md5" : "sha256";
	for (int e = 0; e < cli->errc; ++e)
		fprintf(stderr, "ft_ssl: %s: %s: No such file or directory\n", name, cli->errv[e]);
	(void)argv; /* argv currently unused except for program name; keep signature simple */
}

/* ─────────────── entry ─────────────── */

int main(int argc, char **argv)
{
	if (argc < 2)
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);
	t_option opt = parse_cmd(argv[1]);
	if (opt == NONE)
		return (fprintf(stderr, "ft_ssl: Error: '%s' is an invalid command", argv[1]), 1);
	/* command only; if stdin is piped, hash it with default formatting */
	if (argc < 3)
	{
		if (!isatty(STDIN_FILENO))
		{
			if (opt == MD5)
				manageMD5(NULL, NULL);
			else
				manageSHA256(NULL, NULL);
			return 0;
		}
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);
	}

	t_cli cli;
	parse_cli(argc, argv, &cli);

	if (opt == MD5)
		dispatch_md5(&cli);
	else
		dispatch_sha256(&cli);

	fflush(stdout);
	print_errors(opt, argv, &cli);
	return 0;
}
