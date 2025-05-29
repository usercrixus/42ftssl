#include "openssl.h"

t_option parse(char *command)
{
	if (strcmp(command, "md5") == 0)
		return (MD5);
	else if (strcmp(command, "sha256") == 0)
		return (SHA256);
	return (NONE);
}

int main(int argc, char **argv)
{
	char *flags;
	char *value;

	if (argc < 3)
		return (1);

	t_option option = parse(argv[1]);
	if (argv[2][0] == '-')
	{
		flags = argv[2];
		if (argc == 4)
			value = argv[3];
	}
	else
	{
		value = argv[2];
		flags = 0;
	}
	if (option == MD5)
		manageMD5(flags, value);
	// else if (option == SHA256)
	// 	manageSHA256();
	return (0);
}
