## ft_ssl

Minimal OpenSSL-like hashing utility implementing MD5 and SHA256. Supports flags `-p`, `-q`, `-r`, `-s` and input from stdin, string, and file.

Build

- From repo root: `make`
- Useful targets: `make clean`, `make fclean`, `make re`, `make test`
- Binary: `./ft_ssl` (submodules/libft are handled by the Makefile)

Tests

- Quick: `make test` (runs valgrind + `test/test_ft_ssl.sh`)
- Also compares against `openssl`, `md5sum`, `sha256sum` when available

Usage

- General form: `./ft_ssl <md5|sha256> [FLAGS] [FILE]`
- With no args and when piped, reads and hashes stdin.

Flags

- `-p`: read stdin, echo it back, then print the digest
- `-q`: quiet mode, print only the digest
- `-r`: reversed format (e.g., `digest filename`)
- `-s <str>`: hash the given literal string
- Flags can be grouped: `-pqr`

MD5 Examples

- Stdin: `echo "42 is nice" | ./ft_ssl md5`
- Stdin + echo: `echo "42 is nice" | ./ft_ssl md5 -p`
- String: `./ft_ssl md5 -s "foo"`
- File (quiet): `./ft_ssl md5 -q file`
- File (reversed): `./ft_ssl md5 -r file`

SHA256 Examples

- Stdin: `echo "42 is nice" | ./ft_ssl sha256`
- Stdin + echo: `echo "42 is nice" | ./ft_ssl sha256 -p`
- String: `./ft_ssl sha256 -s "foo"`
- File (quiet): `./ft_ssl sha256 -q file`
- File (reversed): `./ft_ssl sha256 -r file`

Notes

- Processing order: `-p` (stdin) → `-s` (string) → file.
- With `-q`, only the digest is printed (labels and `-r` formatting are ignored).
- The test script expects a POSIX shell and coreutils.
