#include <string.h>
#include <stdio.h>
#include "sha256.h"
#include "sha256Encode.h"

// encode/decode little<->big helpers (SHA-256 uses big-endian)
static void be32enc(unsigned char out[4], uint32_t x)
{
    out[0] = (unsigned char)(x >> 24);
    out[1] = (unsigned char)(x >> 16);
    out[2] = (unsigned char)(x >> 8);
    out[3] = (unsigned char)(x);
}
static uint32_t be32dec(const unsigned char in[4])
{
    return ((uint32_t)in[0] << 24) | ((uint32_t)in[1] << 16) | ((uint32_t)in[2] << 8) | ((uint32_t)in[3]);
}

static void sha256_process_block(uint32_t state[8], const unsigned char block[64])
{
    uint32_t W[64];
    for (int i = 0; i < 16; ++i)
        W[i] = be32dec(&block[i * 4]);
    for (int i = 16; i < 64; ++i)
        W[i] = SSIG1(W[i - 2]) + W[i - 7] + SSIG0(W[i - 15]) + W[i - 16];

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];

    for (int i = 0; i < 64; ++i)
    {
        uint32_t T1 = h + BSIG1(e) + Ch(e, f, g) + K256[i] + W[i];
        uint32_t T2 = BSIG0(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;

    // wipe
    memset(W, 0, sizeof(W));
}

void SHA256Init(SHA256_CONTEXT *ctx)
{
    ctx->totalBit = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

void SHA256Update(SHA256_CONTEXT *ctx, const unsigned char *input, uint64_t len)
{
    uint64_t index = (ctx->totalBit / 8) % 64;
    ctx->totalBit += len * 8;

    uint64_t available = 64 - index;
    uint64_t i = 0;

    if (len >= available)
    {
        memcpy(&ctx->buffer[index], input, available);
        sha256_process_block(ctx->state, ctx->buffer);
        for (i = available; i + 63 < len; i += 64)
            sha256_process_block(ctx->state, &input[i]);
        index = 0;
    }
    memcpy(&ctx->buffer[index], &input[i], len - i);
}

void SHA256Final(unsigned char digest[32], SHA256_CONTEXT *ctx)
{
    unsigned char pad[64] = {0x80};
    unsigned char len_be[8];
    uint64_t total = ctx->totalBit;

    // 64-bit big-endian length
    for (int i = 0; i < 8; ++i)
        len_be[7 - i] = (unsigned char)(total >> (i * 8));

    uint64_t index = (total / 8) % 64;
    uint64_t padLen = (index < 56) ? (56 - index) : (120 - index);

    SHA256Update(ctx, pad, padLen);
    SHA256Update(ctx, len_be, 8);

    for (int i = 0; i < 8; ++i)
        be32enc(&digest[i * 4], ctx->state[i]);

    // wipe context
    memset(ctx, 0, sizeof(*ctx));
}
