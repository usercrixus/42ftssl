#include "md5.h"
#include <unistd.h>

/* ───────────────────────── helpers ───────────────────────── */

static void print_hex(const unsigned char *d, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		printf("%02x", d[i]);
}

static unsigned char *read_all_stdin(size_t *out_len)
{
	size_t cap = 4096, len = 0;
	unsigned char *buf = malloc(cap);
	if (!buf)
	{
		perror("malloc");
		exit(1);
	}
	unsigned char tmp[4096];
	ssize_t n;
	while ((n = read(STDIN_FILENO, tmp, sizeof(tmp))) > 0)
	{
		if (len + (size_t)n > cap)
		{
			cap = (len + (size_t)n) * 2;
			unsigned char *nb = realloc(buf, cap);
			if (!nb)
			{
				free(buf);
				perror("realloc");
				exit(1);
			}
			buf = nb;
		}
		memcpy(buf + len, tmp, (size_t)n);
		len += (size_t)n;
	}
	if (n < 0)
	{
		perror("read");
		free(buf);
		exit(1);
	}
	*out_len = len;
	return buf;
}

static void md5_digest_bytes(const unsigned char *data, size_t len, unsigned char out[16])
{
	MD5_CONTEXT ctx;
	MD5Init(&ctx);
	if (len)
		MD5Update(&ctx, data, (uint64_t)len);
	MD5Final(out, &ctx);
}

/* ─────────────────────── mode: string ─────────────────────── */

static void do_string(char *flags, const char *input, const char *title)
{
	unsigned char dig[16];
	md5_digest_bytes((const unsigned char *)input, strlen(input), dig);

	if (flags && isFlagSet(flags, 'q'))
	{
		print_hex(dig, 16);
		printf("\n");
	}
	else if (flags && isFlagSet(flags, 'r'))
	{
		print_hex(dig, 16);
		printf(" \"%s\"\n", input);
	}
	else
	{
		printf("MD5 (\"%s\") = ", title);
		print_hex(dig, 16);
		printf("\n");
	}
}

/* ───────────────────────── mode: file ─────────────────────── */

static void do_file(char *flags, const char *path)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		perror(path);
		exit(1);
	}

	unsigned char buf[4096], dig[16];
	ssize_t n;
	MD5_CONTEXT ctx;
	MD5Init(&ctx);
	while ((n = read(fd, buf, sizeof(buf))) > 0)
		MD5Update(&ctx, buf, (uint64_t)n);
	if (n < 0)
	{
		perror("read");
		close(fd);
		exit(1);
	}
	close(fd);
	MD5Final(dig, &ctx);

	if (flags && isFlagSet(flags, 'q'))
	{
		print_hex(dig, 16);
		printf("\n");
	}
	else if (flags && isFlagSet(flags, 'r'))
	{
		print_hex(dig, 16);
		printf(" %s\n", path);
	}
	else
	{
		printf("MD5 (%s) = ", path);
		print_hex(dig, 16);
		printf("\n");
	}
}

/* ──────────────────────── mode: stdin ─────────────────────── */

static void do_stdin(char *flags)
{
	const int has_p = (flags && isFlagSet(flags, 'p'));
	const int has_q = (flags && isFlagSet(flags, 'q'));

	size_t len = 0;
	unsigned char *all = read_all_stdin(&len);

	unsigned char dig[16];
	/* MD5 policy: hash bytes exactly as read (no trimming). */
	md5_digest_bytes(all, len, dig);

	if (has_p && has_q)
	{
		fwrite(all, 1, len, stdout);
		if (len == 0 || all[len - 1] != '\n')
			printf("\n");
		print_hex(dig, 16);
		printf("\n");
	}
	else if (has_p)
	{
		/* print: ("...")= <hex>, and hash was with the original bytes */
		printf("(\"");
		size_t print_len = len;
		if (print_len && all[print_len - 1] == '\n')
			print_len--; /* visual nicety */
		fwrite(all, 1, print_len, stdout);
		printf("\")= ");
		print_hex(dig, 16);
		printf("\n");
	}
	else if (has_q)
	{
		print_hex(dig, 16);
		printf("\n");
	}
	else
	{
		printf("(stdin)= ");
		print_hex(dig, 16);
		printf("\n");
	}

	free(all);
}

/* ───────────────────────── public API ─────────────────────── */

void manageMD5(char *flags, char *input)
{
	const int has_p = (flags && isFlagSet(flags, 'p'));
	const int has_s = (flags && isFlagSet(flags, 's'));
	const int piped = !isatty(STDIN_FILENO);

	if (!input && !has_s && piped)
	{
		do_stdin(flags);
		return;
	}
	if (has_p && input == NULL)
	{
		do_stdin(flags);
	}
	if (has_s && input)
	{
		do_string(flags, input, input);
		return;
	}
	if (input)
	{
		do_file(flags, input);
	}
}
