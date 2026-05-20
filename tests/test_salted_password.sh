#!/usr/bin/env bash
set -euo pipefail

make salted_password_hash >/dev/null

HASH_FILE_1="test_password_salted_1.hash"
HASH_FILE_2="test_password_salted_2.hash"
rm -f "$HASH_FILE_1" "$HASH_FILE_2"

./salted_password_hash register "same-password" "$HASH_FILE_1" >/dev/null
./salted_password_hash register "same-password" "$HASH_FILE_2" >/dev/null

[[ -s "$HASH_FILE_1" && -s "$HASH_FILE_2" ]] || {
  echo "[FAIL] Salted password hash files were not created"
  exit 1
}

if cmp -s "$HASH_FILE_1" "$HASH_FILE_2"; then
  echo "[FAIL] Same password should not produce the same salted hash record"
  exit 1
fi

./salted_password_hash verify "same-password" "$HASH_FILE_1" >/dev/null || {
  echo "[FAIL] Correct password should verify successfully with saved salt"
  exit 1
}

if ./salted_password_hash verify "wrong password / sai mật khẩu" "$HASH_FILE_1" >/dev/null; then
  echo "[FAIL] Wrong password should be rejected in salted verify"
  exit 1
fi

echo "[PASS] Salted password test passed."
