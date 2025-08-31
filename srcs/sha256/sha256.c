#include "sha256.h"
#include "sha256Encode.h"
#include "../helper/parse.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../helper/utils.h"
#include "../../42libft/ft_printf/ft_printf.h"

static void sha256_digest_bytes(const unsigned char *data, size_t len, unsigned char out[32])
{
    SHA256_CONTEXT ctx;
    SHA256Init(&ctx);
    if (len)
        SHA256Update(&ctx, data, (uint64_t)len);
    SHA256Final(out, &ctx);
}

/* ─────────────────────── mode: string ─────────────────────── */

static void do_string(char *flags, const char *input, const char *title)
{
    unsigned char dig[32];
    sha256_digest_bytes((const unsigned char *)input, strlen(input), dig);

    if (flags && isFlagSet(flags, 'q'))
    {
        print_hex(dig, 32);
        ft_printf("\n");
    }
    else if (flags && isFlagSet(flags, 'r'))
    {
        print_hex(dig, 32);
        ft_printf(" \"%s\"\n", input);
    }
    else
    {
        ft_printf("SHA256 (\"%s\") = ", title);
        print_hex(dig, 32);
        ft_printf("\n");
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

    unsigned char buf[4096], dig[32];
    ssize_t n;
    SHA256_CONTEXT ctx;
    SHA256Init(&ctx);
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        SHA256Update(&ctx, buf, (uint64_t)n);
    if (n < 0)
    {
        int e = errno;
        close(fd);
        errno = e;
        perror("read");
        exit(1);
    }
    close(fd);
    SHA256Final(dig, &ctx);

    if (flags && isFlagSet(flags, 'q'))
    {
        print_hex(dig, 32);
        ft_printf("\n");
    }
    else if (flags && isFlagSet(flags, 'r'))
    {
        print_hex(dig, 32);
        ft_printf(" %s\n", path);
    }
    else
    {
        ft_printf("SHA256 (%s) = ", path);
        print_hex(dig, 32);
        ft_printf("\n");
    }
}

/* ──────────────────────── mode: stdin ─────────────────────── */

static void do_stdin(char *flags)
{
    const int has_p = (flags && isFlagSet(flags, 'p'));
    const int has_q = (flags && isFlagSet(flags, 'q'));

    size_t len = 0;
    unsigned char *all = read_all_stdin(&len);

    /* SHA-256 policy: with -p, trim exactly one trailing '\n' from the hash input (keep echo intact). */
    size_t hash_len = len;
    if (has_p && hash_len && all[hash_len - 1] == '\n')
        hash_len -= 1;

    unsigned char dig[32];
    sha256_digest_bytes(all, hash_len, dig);

    if (has_p && len)
    {
        if (write(STDOUT_FILENO, all, len) < 0)
        {
            perror("write");
            free(all);
            exit(1);
        }
    }

    if (has_q)
    {
        if (has_p && (!len || all[len - 1] != '\n'))
            ft_printf("\n"); /* match MD5 nicety */
        print_hex(dig, 32);
        ft_printf("\n");
    }
    else if (has_p)
    {
        print_hex(dig, 32);
        ft_printf("\n");
    }
    else
    {
        ft_printf("(stdin)= ");
        print_hex(dig, 32);
        ft_printf("\n");
    }

    free(all);
}

/* ───────────────────────── public API ─────────────────────── */

void manageSHA256(char *flags, char *input)
{
    const int has_p = (flags && isFlagSet(flags, 'p'));
    const int has_s = (flags && isFlagSet(flags, 's'));
    const int piped = !isatty(STDIN_FILENO);

    if (!input && !has_s && piped)
        do_stdin(flags);
    else if (has_p && input == NULL)
        do_stdin(flags);
    else if (has_s && input)
        do_string(flags, input, input);
    else if (input)
        do_file(flags, input);
}
