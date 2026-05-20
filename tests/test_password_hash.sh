#!/usr/bin/env bash
set -euo pipefail

make password_hash >/dev/null

HASH_FILE="test_password.hash"
rm -f "$HASH_FILE"

./password_hash register "student-password-123" "$HASH_FILE" >/dev/null
[[ -s "$HASH_FILE" ]] || { echo "[FAIL] Password hash file was not created"; exit 1; }

./password_hash verify "student-password-123" "$HASH_FILE" >/dev/null || {
  echo "[FAIL] Correct password should verify successfully"
  exit 1
}

if ./password_hash verify "wrong password / sai mật khẩu" "$HASH_FILE" >/dev/null; then
  echo "[FAIL] Wrong password should be rejected"
  exit 1
fi

echo "[PASS] Password hash and wrong password negative test passed."
