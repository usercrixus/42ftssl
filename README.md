## ft_ssl

Implémentation minimale de l’utilitaire de hachage façon openssl, avec MD5 et SHA256. Comporte la gestion des flags -p, -q, -r, -s ainsi que l’entrée par stdin, chaîne et fichier.

Compilation

- Depuis la racine: `make`
- Cibles utiles: `make clean`, `make fclean`, `make re`, `make test`
- Le binaire est `./ft_ssl` (les sous-modules/libft sont gérés par le Makefile)

Tests

- Rapide: `make test` (lance valgrind + script `test/test_ft_ssl.sh`)
- Le script compare aussi avec `openssl`, `md5sum`, `sha256sum` si présents

Usage

- Forme générale: `./ft_ssl <md5|sha256> [FLAGS] [FICHIER]`
- Sans arguments et avec un pipe, lit et hache stdin.

Flags

- `-p`: lit stdin, réimprime tel quel puis affiche le digest
- `-q`: mode silencieux, n’affiche que le digest
- `-r`: format inversé (ex. `digest fichier`)
- `-s <str>`: hache la chaîne littérale donnée
- Les flags peuvent être groupés: `-pqr`

Exemples MD5

- Stdin: `echo "42 is nice" | ./ft_ssl md5`
- Stdin + écho: `echo "42 is nice" | ./ft_ssl md5 -p`
- Chaîne: `./ft_ssl md5 -s "foo"`
- Fichier silencieux: `./ft_ssl md5 -q file`
- Fichier inversé: `./ft_ssl md5 -r file`

Exemples SHA256

- Stdin: `echo "42 is nice" | ./ft_ssl sha256`
- Stdin + écho: `echo "42 is nice" | ./ft_ssl sha256 -p`
- Chaîne: `./ft_ssl sha256 -s "foo"`
- Fichier silencieux: `./ft_ssl sha256 -q file`
- Fichier inversé: `./ft_ssl sha256 -r file`

Notes

- L’ordre de traitement est: `-p` (stdin) → `-s` (chaîne) → fichier.
- Avec `-q`, seul le digest est imprimé (les labels/`-r` sont ignorés).
- Le script de tests attend un shell POSIX et coreutils.
