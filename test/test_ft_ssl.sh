#!/usr/bin/env bash
# Minimal per-line tester with tolerant regex for openssl/md5sum spacing.

FTSSL="./ft_ssl"        # adjust if needed

pass=0
fail=0

mkfixtures() {
  echo "And above all," > file
  echo "https://www.42.fr/" > website
}

run() {
  # $1 = human label, $2 = command string, $3 = expected regex (PCRE, multiline)
  local label="$1" cmd="$2" re="$3"
  local out
  out="$(bash -c "$cmd" 2>&1 | tr -d '\r')"

  if printf '%s\0' "$out" | grep -Pzoq "^${re}\z"; then
    echo "[OK]  $label"
    pass=$((pass+1))
  else
    echo "[KO]  $label"
    echo "  got:      $out"
    echo "  expected: /${re}/"
    fail=$((fail+1))
  fi
}

mkfixtures

# --- MD5 tests ---

# 1) openssl md5: allow both '(stdin)=' and 'MD5(stdin)='
run 'openssl md5 (stdin)' \
  'echo "42 is nice" | openssl md5' \
  '(MD5)?\(stdin\)= 35f1d6de0302e2086a4e472266efb3a9'

# 2) md5sum: GNU uses two spaces before filename / '-'
run 'md5sum (stdin)' \
  'echo "42 is nice" | md5sum' \
  '35f1d6de0302e2086a4e472266efb3a9  -'

# 3) ft_ssl md5 reading stdin (no flags)
run './ft_ssl md5 (stdin)' \
  'echo "42 is nice" | '"$FTSSL"' md5' \
  '\(stdin\)= 35f1d6de0302e2086a4e472266efb3a9'

# 4) ft_ssl md5 -p: exact line
run './ft_ssl md5 -p (stdin echo + hash)' \
  'echo "42 is nice" | '"$FTSSL"' md5 -p' \
  '\("42 is nice"\)= 35f1d6de0302e2086a4e472266efb3a9'

# 5) -q -r
run './ft_ssl md5 -q -r' \
  'echo "Pity the living." | '"$FTSSL"' md5 -q -r' \
  'e20c3b973f63482a778f3fd1869b7f25'

# 6) file default
run './ft_ssl md5 file' \
  '"'$FTSSL'" md5 file' \
  'MD5 \(file\) = 53d53ea94217b259c11a5a2d104ec58a'

# 7) file -r
run './ft_ssl md5 -r file' \
  '"'$FTSSL'" md5 -r file' \
  '53d53ea94217b259c11a5a2d104ec58a file'

# 8) -s "…"
run './ft_ssl md5 -s "pity…spotify."' \
  '"'$FTSSL'" md5 -s "pity those that aren'\''t following baerista on spotify."' \
  'MD5 \("pity those that aren'\''t following baerista on spotify\."\) = a3c990a1964705d9bf0e602f44572f5f'

# 9) echo to -p file (two lines)
run './ft_ssl md5 -p file (2 lines)' \
  'echo "be sure to handle edge cases carefully" | '"$FTSSL"' md5 -p file' \
  '\("be sure to handle edge cases carefully"\)= 3553dc7dc5963b583c056d1b9fa3349c\nMD5 \(file\) = 53d53ea94217b259c11a5a2d104ec58a'

# 10) piping something else but hashing file (1 line)
run './ft_ssl md5 file (ignores pipe)' \
  'echo "some of this will not make sense at first" | '"$FTSSL"' md5 file' \
  'MD5 \(file\) = 53d53ea94217b259c11a5a2d104ec58a'

# 11) -p -r file (2 lines)
run './ft_ssl md5 -p -r file (2 lines)' \
  'echo "but eventually you will understand" | '"$FTSSL"' md5 -p -r file' \
  '\("but eventually you will understand"\)= dcdd84e0f635694d2a943fa8d3905281\n53d53ea94217b259c11a5a2d104ec58a file'

# 12) -p then -s then file (3 lines)
run './ft_ssl md5 -p -s "foo" file (3 lines)' \
  'echo "GL HF let'\''s go" | '"$FTSSL"' md5 -p -s "foo" file' \
  '\("GL HF let'\''s go"\)= d1e3cc342b6da09480b27ec57ff243e2\nMD5 \("foo"\) = acbd18db4cc2f85cedef654fccc4a4d8\nMD5 \(file\) = 53d53ea94217b259c11a5a2d104ec58a'

# 13) mixed flags + extra -s (expect 5 lines including 2 errors)
run './ft_ssl md5 -r -p -s "foo" file -s "bar" (errors last)' \
  'echo "one more thing" | '"$FTSSL"' md5 -r -p -s "foo" file -s "bar" 2>&1' \
  '\("one more thing"\)= a0bd1876c6f011dd50fae52827f445f5\nacbd18db4cc2f85cedef654fccc4a4d8 "foo"\n53d53ea94217b259c11a5a2d104ec58a file\nft_ssl: md5: -s: No such file or directory\nft_ssl: md5: bar: No such file or directory'

# 14) -r -q -p -s "foo" file (4 lines; first line is echoed plaintext)
run './ft_ssl md5 -r -q -p -s "foo" file (4 lines)' \
  'echo "just to be extra clear" | '"$FTSSL"' md5 -r -q -p -s "foo" file' \
  'just to be extra clear\n3ba35f1ea0d170cb3b9a752e3360286c\nacbd18db4cc2f85cedef654fccc4a4d8\n53d53ea94217b259c11a5a2d104ec58a'

# --- SHA256 tests already present ---

# 15) sha256 -q website (digest only)
run './ft_ssl sha256 -q website' \
  '"'$FTSSL'" sha256 -q website' \
  '1ceb55d2845d9dd98557b50488db12bbf51aaca5aa9c1199eb795607a2457daf'

# 16) sha256sum website (allow double spaces before filename)
run 'sha256sum website' \
  'sha256sum website' \
  '1ceb55d2845d9dd98557b50488db12bbf51aaca5aa9c1199eb795607a2457daf  website'

# 17) -s "42 is nice"
run './ft_ssl sha256 -s "42 is nice"' \
  '"'$FTSSL'" sha256 -s "42 is nice"' \
  'SHA256 \("42 is nice"\) = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f'

# 18) echo -n | sha256sum (allow two spaces before -)
run 'sha256sum (stdin)' \
  'echo -n "42 is nice" | sha256sum' \
  'b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f  -'

# --- NEW: SHA256 tests mirroring the MD5 suite ---

# 19) ft_ssl sha256 -p (stdin echo + hash)
run './ft_ssl sha256 -p (stdin echo + hash)' \
  'echo "42 is nice" | '"$FTSSL"' sha256 -p' \
  '42 is nice\nb7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f'

# 20) file default
run './ft_ssl sha256 file' \
  '"'$FTSSL'" sha256 file' \
  'SHA256 \(file\) = f9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705'

# 21) file -r
run './ft_ssl sha256 -r file' \
  '"'$FTSSL'" sha256 -r file' \
  'f9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705 file'

# 22) -s "…"
run './ft_ssl sha256 -s "pity…spotify."' \
  '"'$FTSSL'" sha256 -s "pity those that aren'\''t following baerista on spotify."' \
  'SHA256 \("pity those that aren'\''t following baerista on spotify\."\) = 7838c25c9debff86c584245d67b429186d3850c89da31c0b49b8d0380a3e14bf'

# 23) echo to -p file (3 lines: echo, stdin hash, file hash)
run './ft_ssl sha256 -p file (3 lines)' \
  'echo "be sure to handle edge cases carefully" | '"$FTSSL"' sha256 -p file' \
  'be sure to handle edge cases carefully\n785217112b2bbacf59caed1486fd010e9f6e0729bf31cd2f591b282ec10f5964\nSHA256 \(file\) = f9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705'

# 24) piping something else but hashing file (1 line)
run './ft_ssl sha256 file (ignores pipe)' \
  'echo "some of this will not make sense at first" | '"$FTSSL"' sha256 file' \
  'SHA256 \(file\) = f9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705'

# 25) -p -r file (3 lines: echo, stdin hash, reverse file)
run './ft_ssl sha256 -p -r file (3 lines)' \
  'echo "but eventually you will understand" | '"$FTSSL"' sha256 -p -r file' \
  'but eventually you will understand\n1e3c52e4c7fe76ae7486ca0d53ce8466ea04cce5e41249d72c15c280f73db220\nf9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705 file'

# 26) -p then -s then file (4 lines: echo, stdin hash, string, file)
run './ft_ssl sha256 -p -s "foo" file (4 lines)' \
  'echo "GL HF let'\''s go" | '"$FTSSL"' sha256 -p -s "foo" file' \
  'GL HF let'\''s go\nc96827f61bf337bc587c4758377bafab407226a36f8261123643eb2304c87936\nSHA256 \("foo"\) = 2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae\nSHA256 \(file\) = f9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705'

# 27) mixed flags + extra -s (errors last)
run './ft_ssl sha256 -r -p -s "foo" file -s "bar" (errors last)' \
  'echo "one more thing" | '"$FTSSL"' sha256 -r -p -s "foo" file -s "bar" 2>&1' \
  'one more thing\n1e9ab2b08e82e682f2f2a6d7bb29d4ad5a01c9527d1ecde241760f71e5cf22a7\n2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae "foo"\nf9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705 file\nft_ssl: sha256: -s: No such file or directory\nft_ssl: sha256: bar: No such file or directory'

# 28) -r -q -p -s "foo" file (4 lines: echo, stdin hash, string hash, file hash)
run './ft_ssl sha256 -r -q -p -s "foo" file (4 lines)' \
  'echo "just to be extra clear" | '"$FTSSL"' sha256 -r -q -p -s "foo" file' \
  'just to be extra clear\n2716525f352ca213b295bf1b05a7aebc31a114a92f8345a1f636a1223a762759\n2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae\nf9eb9a5a063eb386a18525c074e1065c316ec434f911e0d7d59ba2d9fd134705'

# --- Huge stdin (exactly 1,000,000 'A' bytes) ---
run './ft_ssl md5 huge stdin (1e6)' \
  '(dd if=/dev/zero bs=1000000 count=1 2>/dev/null | tr "\0" "A") | '"$FTSSL"' md5' \
  '\(stdin\)= 48fcdb8b87ce8ef779774199a856091d'

run './ft_ssl sha256 huge stdin (1e6)' \
  '(dd if=/dev/zero bs=1000000 count=1 2>/dev/null | tr "\0" "A") | '"$FTSSL"' sha256' \
  '\(stdin\)= e23c0cda5bcdecddec446b54439995c7260c8cdcf2953eec9f5cdb6948e5898d'

rm "file"
rm "website"

echo
if [[ $fail -eq 0 ]]; then
  echo "✅ OK — $pass tests passed."
  exit 0
else
  echo "❌ KO — $fail failed, $pass passed."
  exit 1
fi