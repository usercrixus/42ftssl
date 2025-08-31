#pragma once
#include <stdbool.h>

typedef struct s_cmd_data
{
	char *flags;	// unique chars in combined flag groups
	char *s_arg;	// first -s argument (if any)
	char *file_arg; // first non-flag argument after flags
	char *errv[32]; // leftover args to report as errors
	int errc;		// error count
} t_cmd_data;

bool isFlagSet(const char *flags, char c);
void parse_data(int argc, char **argv, t_cmd_data *data);
