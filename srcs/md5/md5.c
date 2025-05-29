#include "md5.h"

void manageString(char *flags, char *input, char *title)
{
	unsigned char digest[16]; // 16 octet (128 bit)
	MD5_CONTEXT ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)input, strlen(input));
	MD5Final(digest, &ctx);

	if (flags && isFlagSet(flags, 'r'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf(" %s", input);
	}
	else
	{
		printf("MD5(\"%s\") = ", title);
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
	}
	printf("\n");
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
	if (flags && isFlagSet(flags, 'r'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf(" %s\n", path);
	}
	else
	{
		printf("MD5(%s)= ", path);
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

	MD5Init(&ctx);
	while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
	{
		MD5Update(&ctx, buffer, n);
	}
	if (n < 0)
	{
		perror("read");
		exit(1);
	}
	MD5Final(digest, &ctx);
	if (flags && isFlagSet(flags, 'r'))
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
	else
	{
		for (int i = 0; i < 16; i++)
			printf("%02x", digest[i]);
		printf("\n");
	}
}

void manageMD5(char *flags, char *input)
{
	if (flags && isFlagSet(flags, 'p') && isFlagSet(flags, 's'))
	{
		printf("You can use flag p and s at the same time");
		exit(1);
	}
	else if (flags && isFlagSet(flags, 'p'))
		manageSTDIN(flags);
	else if (flags && isFlagSet(flags, 's'))
		manageString(flags, input, input);
	else
		manageFile(flags, input);
}
