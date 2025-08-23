#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct
{
	uint32_t state[8];		  // H0..H7
	uint64_t totalBit;		  // total bits processed
	unsigned char buffer[64]; // 512-bit block buffer
} SHA256_CONTEXT;

void manageSHA256(char *flags, char *input);
void sha256_manageString(char *flags, char *input, char *title);
void sha256_manageFile(char *flags, const char *path);
void sha256_manageSTDIN(char *flags);
