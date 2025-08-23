#include "sha256.h"
#include "sha256Encode.h"
#include "../parse.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/* -- helpers identical to your existing print style -- */
static void sha256_print_hex(const unsigned char *digest, int bytes)
{
    for (int i = 0; i < bytes; ++i) printf("%02x", digest[i]);
}

/* string mode */
static void manageString(char *flags, const char *input, const char *title)
{
    unsigned char digest[32];
    SHA256_CONTEXT ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, (const unsigned char *)input, (uint64_t)strlen(input));
    SHA256Final(digest, &ctx);

    if (flags && isFlagSet(flags, 'q')) {
        sha256_print_hex(digest, 32);
        printf("\n");
    } else if (flags && isFlagSet(flags, 'r')) {
        sha256_print_hex(digest, 32);
        printf(" \"%s\"\n", input);
    } else {
        printf("SHA256 (\"%s\") = ", title);
        sha256_print_hex(digest, 32);
        printf("\n");
    }
}

/* file mode */
static void manageFile(char *flags, const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror(path); exit(1); }

    unsigned char buffer[4096], digest[32];
    ssize_t n;
    SHA256_CONTEXT ctx;
    SHA256Init(&ctx);
    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
        SHA256Update(&ctx, buffer, (uint64_t)n);
    if (n < 0) { int e = errno; close(fd); errno = e; perror("read"); exit(1); }
    close(fd);
    SHA256Final(digest, &ctx);

    if (flags && isFlagSet(flags, 'q')) {
        sha256_print_hex(digest, 32);
        printf("\n");
    } else if (flags && isFlagSet(flags, 'r')) {
        sha256_print_hex(digest, 32);
        printf(" %s\n", path);
    } else {
        printf("SHA256 (%s) = ", path);
        sha256_print_hex(digest, 32);
        printf("\n");
    }
}

/* stdin mode: buffer so we can echo and (for -p) trim one trailing '\n' from the hash */
static void manageSTDIN(char *flags)
{
    int has_p = (flags && isFlagSet(flags, 'p'));
    int has_q = (flags && isFlagSet(flags, 'q'));

    /* read-all buffering (like your MD5 manageSTDIN) */
    size_t cap = 4096, len = 0;
    unsigned char *all = (unsigned char *)malloc(cap);
    if (!all) { perror("malloc"); exit(1); }

    unsigned char tmp[4096];
    ssize_t n;
    while ((n = read(STDIN_FILENO, tmp, sizeof(tmp))) > 0) {
        if (len + (size_t)n > cap) {
            cap = (len + (size_t)n) * 2;
            unsigned char *nb = (unsigned char *)realloc(all, cap);
            if (!nb) { free(all); perror("realloc"); exit(1); }
            all = nb;
        }
        memcpy(all + len, tmp, (size_t)n);
        len += (size_t)n;
    }
    if (n < 0) { perror("read"); free(all); exit(1); }

    /* echo if -p (exact bytes) */
    if (has_p && len) {
        if (write(STDOUT_FILENO, all, len) < 0) { perror("write"); free(all); exit(1); }
    }

    /* build digest; if -p, trim exactly one trailing '\n' from the *hash input* */
    size_t hash_len = len;
    if (has_p && hash_len && all[hash_len - 1] == '\n') hash_len -= 1;

    unsigned char digest[32];
    SHA256_CONTEXT ctx;
    SHA256Init(&ctx);
    if (hash_len) SHA256Update(&ctx, all, (uint64_t)hash_len);
    SHA256Final(digest, &ctx);

    if (has_q) {
        sha256_print_hex(digest, 32);
        printf("\n");
    } else if (has_p) {
        /* echo was already printed; just the hex line next */
        sha256_print_hex(digest, 32);
        printf("\n");
    } else {
        /* no -p/-q: label like MD5 */
        printf("(stdin)= ");
        sha256_print_hex(digest, 32);
        printf("\n");
    }

    free(all);
}

/* public entry: mirror MD5 flow – only read stdin if this invocation is for stdin */
void manageSHA256(char *flags, char *input)
{
    int has_p = (flags && isFlagSet(flags, 'p'));
    int has_s = (flags && isFlagSet(flags, 's'));
    int stdin_piped = !isatty(STDIN_FILENO);

    /* Pure piped stdin (no -s, no input path) */
    if (!input && !has_s && stdin_piped) {
        manageSTDIN(flags);
        return;
    }

    /* Only read stdin (-p) when this call is for stdin */
    if (has_p && input == NULL)
        manageSTDIN(flags);

    if (has_s && input) {
        manageString(flags, input, input);
        return; /* do not fall through to file */
    }

    if (input)
        manageFile(flags, input);
}
