# ft_ssl

A minimal reimplementation of the classic `openssl` message digest utility.  
It supports **MD5** and **SHA256** hashing, with flag handling (`-p`, `-q`, `-r`, `-s`) and routing logic for stdin, string literals, and files.  
The goal is to replicate the behavior of `openssl`/`md5sum` closely, including error handling and output formatting, while keeping the code clean and structured.


# Build, Run & Test

## Prereqs
- POSIX shell + coreutils (`bash`, `dd`, `tr`, `grep -P`)
- `valgrind` (for memcheck)
- `openssl`, `md5sum`, `sha256sum` (used by the test script for cross-checks)
- Git submodules enabled (for `42libft`)

> macOS note: `md5sum`/`sha256sum` aren’t standard on macOS. Install via Homebrew (`brew install coreutils`) and use `gmd5sum`/`gsha256sum`, or skip the lines in the script that call them.

## Build
```bash
# from repo root
make            # builds ./ft_ssl (brings submodules + libft)
```

Targets:
- `make` / `make all` – build `ft_ssl`
- `make clean` – remove objects
- `make fclean` – remove objects + binary
- `make re` – full rebuild
- `make test` – Full Test Suite

## Quick Usage
```bash
# stdin
echo "42 is nice" | ./ft_ssl md5
echo "42 is nice" | ./ft_ssl sha256

# flags
./ft_ssl md5 -p
./ft_ssl md5 -s "foo"
./ft_ssl md5 -q file
./ft_ssl md5 -r file

./ft_ssl sha256 -p
./ft_ssl sha256 -s "foo"
./ft_ssl sha256 -q file
./ft_ssl sha256 -r file
```

# ft_ssl – Flag & Routing Logic (MD5 / SHA256)

This part explains **how flags are interpreted**, **how inputs are routed** (stdin / `-s` / file), and **how errors are reported**. It intentionally focuses on **logic**, not implementation details.

The behavior below matches the provided test suite exactly.

---

## Commands

- `ft_ssl md5 [FLAGS] [FILE] [EXTRA…]`
- `ft_ssl sha256 [FLAGS] [FILE] [EXTRA…]`

If the first argument is neither `md5` nor `sha256`, an error is printed:
```
ft_ssl: Error: '<cmd>' is an invalid command
```

If the command is present but **no further arguments**:
- If **stdin is piped**, hash **stdin** (labeled `(stdin)= ...`) and exit.
- If **stdin is a TTY**, print usage error and exit.

---

## Supported flags

- `-p` : Read **stdin** and print it **before**/with the digest.
- `-q` : **Quiet** — print **only** the digest (flag formatting suppressed).
- `-r` : **Reverse** formatting — for strings: `hash "str"`, for files: `hash filename`.
- `-s <string>` : Hash an explicit **string literal** (first one only).

> Notes  
> • `-s` consumes **one** following non-flag token as its value.  
> • Flags may be grouped (e.g., `-qrp`).  
> • Flags are **deduplicated** (repeating the same char has no extra effect).

---

## Input sources and routing

There are exactly **three** possible inputs to hash in a single run, processed in this order:

1. **`-p` (stdin)** — if `-p` is set, stdin is read & printed/hashed **first**.
2. **`-s "<string>"`** — if `-s` is set **and** a value was captured, it’s hashed **second**.
3. **`<file>`** — if a file argument is present (first non-flag token after flags), it’s hashed **third**.

Anything **after** the first file token becomes an **error** (see below).

### Important routing rules

- **`-p` and `-s` do not carry over to files.**  
  When hashing a file, we drop `p` and `s` from the flag set (so file formatting is only affected by e.g. `-q`, `-r`).

- **Piped stdin is ignored if a file is given**, **unless** you explicitly used `-p`.  
  In other words, `echo hi | ft_ssl md5 file` hashes **only** `file`.

- **Asymmetry (by design to match tests):**  
  - For **MD5 only**: if you provided flags but **none produced output** (no `-p`, no valid `-s`, no file) **and** stdin is piped, MD5 will still hash piped stdin as a final fallback.  
  - For **SHA256**, this final fallback does **not** occur.

---

## Output formatting & precedence

Formatting depends on **what** you’re hashing and the active flags. Precedence is:

1. `-q` (quiet) **overrides** everything about labels and `-r`.  
   You only get the hex digest (plus a newline).
2. If **not** quiet:
   - `-r` switches to reverse formatting:  
     - string: `digest "string"`  
     - file:   `digest filename`
   - Else default formatting:  
     - string: `ALGO ("string") = digest`  
     - file:   `ALGO (filename) = digest`  
     - stdin (no `-p`): `(stdin)= digest` (compat label)

### `-p` printing

- `-p` **prints stdin bytes exactly as received**, then prints the digest line.  
- With `-q -p`, you still get the echoed stdin (exact bytes), then the bare digest.

> The exact newline behavior is tuned to match the tests; you can rely on the test suite as the source of truth for expected lines.

---

## How flags are parsed

- Flags are taken from any leading `-<group>` tokens (e.g., `-p`, `-qr`, `-pqr`…), **deduplicated**, and combined.
- When `-s` is seen for the **first time**, the next **non-flag** token (if any) becomes the **string value** and is consumed.
- After the flag groups:
  - The next **non-flag** token becomes the **file** (if any).
  - **All remaining tokens** (including more `-s` groups or words) are treated as **errors**.

---

## Error handling

After all hashing output is printed, any **leftover tokens** are reported as errors in the following form (one per line):

```
ft_ssl: <algo>: <token>: No such file or directory
```

Example:
```
ft_ssl: sha256: -s: No such file or directory
ft_ssl: sha256: bar: No such file or directory
```

This ordering (outputs first, then errors) is deliberate and matches the tests.

---

## What exactly gets hashed?

- **`-p`**: stdin content.  
- **`-s "<string>"`**: the captured string literal (without surrounding quotes).  
- **`file`**: the file’s bytes.

**Piped stdin is only hashed if:**
- You used **`-p`**, or
- You ran **only the command** with **no further args** while stdin is piped (special case at the top), or
- **MD5 only**: the “final fallback” case described above.

---

## Quick examples (mirroring tests)

- `echo "42 is nice" | ft_ssl md5`  
  → hashes stdin, prints: `(stdin)= <md5>`

- `echo "42 is nice" | ft_ssl md5 -p`  
  → prints the line, then the digest line for stdin.

- `ft_ssl md5 -s "foo"`  
  → `MD5 ("foo") = <digest>`

- `ft_ssl md5 -q -r file`  
  → `<digest>` (quiet beats reverse; file is hashed, no labels)

- `echo "x" | ft_ssl sha256 file`  
  → hashes **file** only (pipe ignored), prints `SHA256 (file) = <digest>`

- `echo "x" | ft_ssl sha256 -p -r file`  
  → three lines: echoed stdin, its digest, then `<digest> file`

- `echo "one more thing" | ft_ssl sha256 -r -p -s "foo" file -s "bar"`  
  → outputs (stdin, string "foo", file), then errors for `-s` and `bar`.

---

## Symmetry between MD5 and SHA256

- **Flag parsing, ordering, file-flag pruning, quiet/reverse formatting, error printing** — all symmetric.
- **One intentional difference** (test-driven):  
  **MD5** performs a last-chance “piped stdin” hash when no other work was dispatched and stdin is piped; **SHA256** does not.

If you later decide you want *perfect* symmetry, remove that MD5-only fallback (or add it to SHA256), but note you’ll need to adjust tests accordingly.

---

## TL;DR flowchart

1) Validate command (`md5`/`sha256`)  
2) If no args after command:  
   - piped stdin → hash stdin; else → usage error  
3) Parse flags (`-p`, `-q`, `-r`, first `-s` value), capture **one** file, collect extras as **errors**  
4) Dispatch in order:  
   - `-p` (stdin) → output  
   - `-s` (string) → output  
   - `file` → output (with `p`/`s` dropped)  
   - **MD5 only** final stdin fallback if nothing ran and stdin is piped  
5) Print accumulated **errors** (one per line)