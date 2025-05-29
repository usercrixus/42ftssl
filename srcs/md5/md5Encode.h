#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// Transformation macros for rounds
#define FF(a, b, c, d, x, s, ac)                        \
	{                                                   \
		(a) += F((b), (c), (d)) + (x) + (uint32_t)(ac); \
		(a) = ROTATE_LEFT((a), (s));                    \
		(a) += (b);                                     \
	}
#define GG(a, b, c, d, x, s, ac)                        \
	{                                                   \
		(a) += G((b), (c), (d)) + (x) + (uint32_t)(ac); \
		(a) = ROTATE_LEFT((a), (s));                    \
		(a) += (b);                                     \
	}
#define HH(a, b, c, d, x, s, ac)                        \
	{                                                   \
		(a) += H((b), (c), (d)) + (x) + (uint32_t)(ac); \
		(a) = ROTATE_LEFT((a), (s));                    \
		(a) += (b);                                     \
	}
#define II(a, b, c, d, x, s, ac)                        \
	{                                                   \
		(a) += I((b), (c), (d)) + (x) + (uint32_t)(ac); \
		(a) = ROTATE_LEFT((a), (s));                    \
		(a) += (b);                                     \
	}

// MD5 context structure
typedef struct
{
	uint32_t state[4];		  // A, B, C, D
	uint64_t totalBit;		  // number of bits to encoded so far
	unsigned char buffer[64]; // input buffer
} MD5_CONTEXT;

// Function prototypes
void MD5Init(MD5_CONTEXT *ctx);
void MD5Update(MD5_CONTEXT *ctx, const unsigned char *input, uint64_t len);
void MD5Final(unsigned char digest[16], MD5_CONTEXT *ctx);

// Constants for MD5Transform routine.
enum
{
	S11 = 7,
	S12 = 12,
	S13 = 17,
	S14 = 22,
	S21 = 5,
	S22 = 9,
	S23 = 14,
	S24 = 20,
	S31 = 4,
	S32 = 11,
	S33 = 16,
	S34 = 23,
	S41 = 6,
	S42 = 10,
	S43 = 15,
	S44 = 21
};