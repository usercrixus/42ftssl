#include "parse.h"

bool isFlagSet(char *flags, char c)
{
	int j;

	j = 0;
	while (flags[j])
	{
		if (flags[j] == c)
			return (true);
		j++;
	}
	return (false);
}

bool parseCommand(char* command)
{
	if (strcmp(command, "md5") == 0)
		return (MD5);
	else if (strcmp(command, "sha256") == 0)
		return (SHA256);
	return (NONE);
	return (false);
}
