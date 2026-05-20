#!/usr/bin/env bash
set -euo pipefail

make all

echo "== 1. Known answer tests =="
./sha256 --self-test

echo "== 2. Hash string =="
./sha256 --hash-string "hello FIT4012 SHA"

echo "== 3. File integrity =="
printf "FIT4012 SHA file integrity sample\n" > sample.txt
EXPECTED_HASH=$(./sha256 --hash-file sample.txt)
./file_integrity sample.txt "$EXPECTED_HASH"
printf "tampered\n" >> sample.txt
if ./file_integrity sample.txt "$EXPECTED_HASH"; then
  echo "[FAIL] Tamper case should fail"
  exit 1
else
  echo "[PASS] Tamper case detected"
fi

echo "== 4. Password hash =="
./password_hash register "fit4012-demo-password" test_password.hash
./password_hash verify "fit4012-demo-password" test_password.hash
if ./password_hash verify "wrong-password" test_password.hash; then
  echo "[FAIL] Wrong password should fail"
  exit 1
else
  echo "[PASS] Wrong password rejected"
fi

echo "== 5. Salted password hash =="
./salted_password_hash register "fit4012-demo-password" test_password_salted_1.hash
./salted_password_hash register "fit4012-demo-password" test_password_salted_2.hash
./salted_password_hash verify "fit4012-demo-password" test_password_salted_1.hash
if cmp -s test_password_salted_1.hash test_password_salted_2.hash; then
  echo "[FAIL] Same password should produce different salted records"
  exit 1
else
  echo "[PASS] Same password produced different salted records"
fi
