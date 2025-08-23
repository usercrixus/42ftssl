#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "openssl.h"
#include "parse.h"

static t_option parse_cmd(const char *command)
{
	if (!command)
		return NONE;
	if (strcmp(command, "md5") == 0)
		return MD5;
	if (strcmp(command, "sha256") == 0)
		return SHA256;
	return NONE;
}

static const char *flags_without_many(const char *flags, const char *drop)
{
	static char buf[64];
	if (!flags)
		return NULL;
	size_t k = 0;
	for (size_t i = 0; flags[i] && k + 1 < sizeof(buf); ++i)
	{
		if (!strchr(drop, flags[i]))
			buf[k++] = flags[i];
	}
	buf[k] = '\0';
	return (k ? buf : NULL);
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);

	t_option option = parse_cmd(argv[1]);
	if (option == NONE)
		return (fprintf(stderr, "ft_ssl: Error: '%s' is an invalid command", argv[1]), 1);

	/* Special case: command + (possibly piped) stdin, no other args */
	if (argc < 3)
	{
		if (!isatty(STDIN_FILENO))
		{
			if (option == MD5)
			{
				manageMD5(NULL, NULL);
				return 0;
			}
			if (option == SHA256)
			{
				manageSHA256(NULL, NULL);
				return 0;
			}
		}
		return (fprintf(stderr, "Bad usage, need at least 2 args\n"), 1);
	}

	/* Parse flags: accumulate unique chars; capture first -s value; 1 file; rest -> errors */
	char *s_arg = NULL;
	char *file_arg = NULL;
	char *errv[32];
	int errc = 0;
	char *flags = NULL;
	static char flags_buf[64];

	if (argv[2][0] == '-')
	{
		flags_buf[0] = '\0';
		int i = 2;
		while (i < argc && argv[i][0] == '-')
		{
			const char *p = argv[i] + 1;
			while (*p)
			{
				if (!strchr(flags_buf, *p))
				{
					size_t L = strlen(flags_buf);
					if (L + 2 < sizeof(flags_buf))
					{
						flags_buf[L] = *p;
						flags_buf[L + 1] = '\0';
					}
				}
				if (*p == 's')
				{
					if (i + 1 < argc && argv[i + 1][0] != '-')
					{
						s_arg = argv[i + 1];
						i++;
					}
				}
				p++;
			}
			i++;
		}
		if (flags_buf[0])
			flags = flags_buf;
		if (i < argc && argv[i][0] != '-')
		{
			file_arg = argv[i++];
		}
		for (int j = i; j < argc && errc < (int)(sizeof(errv) / sizeof(errv[0])); ++j)
			errv[errc++] = argv[j];
	}
	else
	{
		file_arg = argv[2];
	}

	/* Dispatch in required order */
	if (option == MD5)
	{
		if (isFlagSet(flags, 'p'))
			manageMD5(flags, NULL);
		if (isFlagSet(flags, 's') && s_arg)
			manageMD5(flags, s_arg);
		if (file_arg)
		{
			/* do not carry -s to files; -p is stdin-only, so drop it too */
			const char *file_flags = flags_without_many(flags, "sp");
			manageMD5((char *)file_flags, file_arg);
		}
		/* Allow pure piped stdin without explicit -p when nothing else was dispatched */
		if (!isFlagSet(flags, 'p') && !(isFlagSet(flags, 's') && s_arg) && !file_arg && !isatty(STDIN_FILENO))
			manageMD5(flags, NULL);
	}
	else
	{
		if (isFlagSet(flags, 'p'))
			manageSHA256(flags, NULL);
		if (isFlagSet(flags, 's') && s_arg)
			manageSHA256(flags, s_arg);
		if (file_arg)
		{
			/* same logic for sha256 */
			const char *file_flags = flags_without_many(flags, "sp");
			manageSHA256((char *)file_flags, file_arg);
		}
	}
	fflush(stdout);
	for (int e = 0; e < errc; ++e)
		fprintf(stderr, "ft_ssl: %s: %s: No such file or directory\n", argv[1], errv[e]);
	return 0;
}
