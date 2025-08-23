#include "md5.h"
#include <unistd.h>

void manageString(char *flags, char *input, char *title)
{
	unsigned char digest[16]; // 16 octet (128 bit)
	MD5_CONTEXT ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)input, strlen(input));
	MD5Final(digest, &ctx);

	if (flags && isFlagSet(flags, 'q'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else if (flags && isFlagSet(flags, 'r'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf(" \"%s\"\n", input); // quoted
	}
	else
	{
		printf("MD5 (\"%s\") = ", title);
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
}

void manageFile(char *flags, const char *path)
{
	int fd;
	long n;
	unsigned char buffer[1024];
	unsigned char digest[16];
	MD5_CONTEXT ctx;

	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		perror(path);
		exit(1);
	}
	MD5Init(&ctx);
	while ((n = read(fd, buffer, sizeof(buffer))) > 0)
		MD5Update(&ctx, buffer, n);
	if (n < 0)
	{
		perror("read");
		exit(1);
	}
	close(fd);
	MD5Final(digest, &ctx);
	if (flags && isFlagSet(flags, 'q'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else if (flags && isFlagSet(flags, 'r'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf(" %s\n", path);
	}
	else
	{
		printf("MD5 (%s) = ", path);
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
}

void manageSTDIN(char *flags)
{
	long n;
	unsigned char buffer[1024];
	unsigned char digest[16];
	MD5_CONTEXT ctx;

	// buffer stdin for -p echo
	size_t cap = 4096, len = 0;
	unsigned char *all = malloc(cap);
	if (!all)
	{
		perror("malloc");
		exit(1);
	}

	MD5Init(&ctx);
	while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
	{
		if (len + n > cap)
		{
			cap = (len + n) * 2;
			unsigned char *nb = realloc(all, cap);
			if (!nb)
			{
				perror("realloc");
				free(all);
				exit(1);
			}
			all = nb;
		}
		memcpy(all + len, buffer, n);
		len += n;

		MD5Update(&ctx, buffer, n);
	}
	if (n < 0)
	{
		perror("read");
		free(all);
		exit(1);
	}

	MD5Final(digest, &ctx);

	int has_p = (flags && isFlagSet(flags, 'p'));
	int has_q = (flags && isFlagSet(flags, 'q'));

	if (has_p && has_q)
	{
		// echo raw stdin, then just the hex
		fwrite(all, 1, len, stdout);
		if (len == 0 || all[len - 1] != '\n')
			printf("\n");
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else if (has_p)
	{
		// ("...")= <hex>   (no trailing newline inside the quotes)
		printf("(\"");
		size_t print_len = len;
		if (print_len && all[print_len - 1] == '\n')
			print_len--;
		fwrite(all, 1, print_len, stdout);
		printf("\")= ");
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else if (has_q)
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else
	{
		printf("(stdin)= ");
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}

	free(all);
}

void manageMD5(char *flags, char *input)
{
    int has_p = (flags && isFlagSet(flags, 'p'));
    int has_s = (flags && isFlagSet(flags, 's'));
    int stdin_piped = !isatty(STDIN_FILENO);

    if (!input && !has_s && stdin_piped) {
        manageSTDIN(flags);
        return;
    }

    /* only read stdin (-p) when this invocation is for stdin */
    if (has_p && input == NULL)
        manageSTDIN(flags);

    if (has_s && input) {
        manageString(flags, input, input);
        return;                 // <-- IMPORTANT: don't fall through to file
    }

    if (input)
        manageFile(flags, input);
}


