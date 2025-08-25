#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "md5Encode.h"
#include "../../42libft/ft_base/libft.h"

/**
 * 32-bit words to byte array (little-endian)
 */
static void md5_encode(unsigned char *output, const uint32_t *input, uint64_t len)
{
	uint64_t i = 0;
	uint64_t j = 0;

	while (j < len)
	{
		output[j] = (unsigned char)(input[i]);
		output[j + 1] = (unsigned char)((input[i] >> 8));
		output[j + 2] = (unsigned char)((input[i] >> 16));
		output[j + 3] = (unsigned char)((input[i] >> 24));

		i++;
		j += 4;
	}
}

/**
 * byte array (little-endian) to 32-bit words in host order
 */
static void md5_decode(uint32_t *output, const unsigned char *input, uint64_t len)
{
	uint64_t i = 0;
	uint64_t j = 0;

	while (j < len)
	{
		output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) | (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
		i++;
		j += 4;
	}
}

// Processes a single 64-byte block
static void md5_process_block(uint32_t state[4], const unsigned char block[64])
{
	uint32_t a = state[0];
	uint32_t b = state[1];
	uint32_t c = state[2];
	uint32_t d = state[3];
	uint32_t x[16];

	md5_decode(x, block, 64);

	// Round 1
	FF(a, b, c, d, x[0], S11, 0xd76aa478);
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);
	FF(c, d, a, b, x[2], S13, 0x242070db);
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);
	FF(d, a, b, c, x[5], S12, 0x4787c62a);
	FF(c, d, a, b, x[6], S13, 0xa8304613);
	FF(b, c, d, a, x[7], S14, 0xfd469501);
	FF(a, b, c, d, x[8], S11, 0x698098d8);
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);
	FF(b, c, d, a, x[11], S14, 0x895cd7be);
	FF(a, b, c, d, x[12], S11, 0x6b901122);
	FF(d, a, b, c, x[13], S12, 0xfd987193);
	FF(c, d, a, b, x[14], S13, 0xa679438e);
	FF(b, c, d, a, x[15], S14, 0x49b40821);

	// Round 2
	GG(a, b, c, d, x[1], S21, 0xf61e2562);
	GG(d, a, b, c, x[6], S22, 0xc040b340);
	GG(c, d, a, b, x[11], S23, 0x265e5a51);
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
	GG(a, b, c, d, x[5], S21, 0xd62f105d);
	GG(d, a, b, c, x[10], S22, 0x02441453);
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);
	GG(d, a, b, c, x[14], S22, 0xc33707d6);
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);
	GG(b, c, d, a, x[8], S24, 0x455a14ed);
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
	GG(c, d, a, b, x[7], S23, 0x676f02d9);
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

	// Round 3
	HH(a, b, c, d, x[5], S31, 0xfffa3942);
	HH(d, a, b, c, x[8], S32, 0x8771f681);
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);
	HH(b, c, d, a, x[14], S34, 0xfde5380c);
	HH(a, b, c, d, x[1], S31, 0xa4beea44);
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);
	HH(b, c, d, a, x[6], S34, 0x04881d05);
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);

	// Round 4
	II(a, b, c, d, x[0], S41, 0xf4292244);
	II(d, a, b, c, x[7], S42, 0x432aff97);
	II(c, d, a, b, x[14], S43, 0xab9423a7);
	II(b, c, d, a, x[5], S44, 0xfc93a039);
	II(a, b, c, d, x[12], S41, 0x655b59c3);
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);
	II(c, d, a, b, x[10], S43, 0xffeff47d);
	II(b, c, d, a, x[1], S44, 0x85845dd1);
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II(c, d, a, b, x[6], S43, 0xa3014314);
	II(b, c, d, a, x[13], S44, 0x4e0811a1);
	II(a, b, c, d, x[4], S41, 0xf7537e82);
	II(d, a, b, c, x[11], S42, 0xbd3af235);
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
	II(b, c, d, a, x[9], S44, 0xeb86d391);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	ft_memset(x, 0, sizeof(x)); // Clear sensitive data
}

/**
 * Initializes MD5 context with magic number
 */
void MD5Init(MD5_CONTEXT *ctx)
{
	ctx->totalBit = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ft_memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

/**
 * Manage the whole process of block encoding
 */
void MD5Update(MD5_CONTEXT *ctx, const unsigned char *input, uint64_t inputLen)
{
	uint64_t index = (ctx->totalBit / 8) % 64; // index of the buffer (as it can have been previously partialy fill)
	ctx->totalBit += (uint64_t)inputLen * 8;   // add the len to the string to encode
	uint64_t availableLen = 64 - index;		   // get the available len of the buffer
	uint64_t i = 0;
	if (inputLen >= availableLen)
	{
		ft_memcpy(&ctx->buffer[index], input, availableLen); // process the first element separatly, because there is maybe a remaining the the buffer
		md5_process_block(ctx->state, ctx->buffer);
		for (i = availableLen; i + 63 < inputLen; i += 64)
		{
			md5_process_block(ctx->state, &input[i]);
		}
		index = 0;
	}
	ft_memcpy(&ctx->buffer[index], &input[i], inputLen - i); // load the remaining in the buffer
}

// Applies padding and appends length, finalizes digest
static void md5_finalize(MD5_CONTEXT *ctx, unsigned char digest[16])
{
	uint64_t originalLen = ctx->totalBit;
	unsigned char padding[64] = {0x80};							   // create an array of bit like 1000000[..] which corresponds to the single “1” bit MD5 requires at the start of the padding.
	uint64_t index = (originalLen / 8) % 64;					   // total number of byte processed so far % 64
	uint64_t padLen = (index < 56) ? (56 - index) : (120 - index); // leave exactly 8 bit at the end of the last block to process
	MD5Update(ctx, padding, padLen);

	unsigned char bits[8];						   // 8 bit buffer for 64 bit len (uint 64)
	md5_encode(bits, (uint32_t *)&originalLen, 8); // writes those two words into bits[] in little-endian order
	MD5Update(ctx, bits, 8);

	md5_encode(digest, ctx->state, 16);

	ft_memset(ctx, 0, sizeof(*ctx)); // Clear sensitive data
}

// Ends an MD5 message-digest operation, writing the digest
void MD5Final(unsigned char digest[16], MD5_CONTEXT *ctx)
{
	md5_finalize(ctx, digest);
}
