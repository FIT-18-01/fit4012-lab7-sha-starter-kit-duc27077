# FIT4012 Lab 7 - Báo cáo 1 trang: SHA-256

## 1. Mục tiêu / Objective

Bài thực hành nhằm phân tích và triển khai thuật toán SHA-256, bao gồm các mục tiêu cụ thể:
- Hiểu và triển khai thuật toán SHA-256 từ đầu
- Chạy chương trình băm chuỗi và kiểm tra với known answer test vectors
- Kiểm tra toàn vẹn file bằng SHA-256
- Triển khai băm mật khẩu và lưu hash
- Cải tiến bảo mật bằng salt để tránh việc hai người có cùng mật khẩu tạo ra cùng hash

## 2. Cách làm / Approach

Nhóm đã thực hiện các bước sau:

- **Biên dịch và chạy `sha_procedure.cpp`**: Sử dụng C++17 để biên dịch, chạy self-test với các test vectors chuẩn
- **Kiểm thử SHA-256**: Sử dụng known answer test vectors (empty string, "abc", "hello FIT4012 SHA") để xác minh tính đúng đắn
- **Viết/chạy chương trình kiểm tra toàn vẹn file (`file_integrity.cpp`)**: Tính hash file, so sánh với hash mong đợi, phát hiện file bị tamper
- **Viết/chạy chương trình băm mật khẩu (`password_hash.cpp`)**: Chế độ register để lưu hash, chế độ verify/login để xác minh
- **Bổ sung salt (`salted_password_hash.cpp`)**: Tạo salt ngẫu nhiên 16 bytes, lưu theo định dạng `salt:hash`, chứng minh cùng mật khẩu nhưng salt khác nhau tạo hash khác nhau

## 3. Kết quả / Result

### Hash của chuỗi `abc`:
```
ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

### Hash của file mẫu trước khi sửa:
```
5ee62dc925a9958dbd6732c570a23c7f65a8c11066e889b15068cfb4bf1a0bd9
```

### Kết quả kiểm tra file sau khi sửa nội dung:
```
[FAIL] File was changed or expected hash is incorrect
expected: 5ee62dc925a9958dbd6732c570a23c7f65a8c11066e889b15068cfb4bf1a0bd9
actual  : c232e5627e703ee5b311e0df8520b3d10dc8867b27636bb58fe58eb1fb9d6acb
[PASS] Tamper case detected
```

### Kết quả xác thực mật khẩu đúng:
```
[PASS] Password verified
```

### Kết quả xác thực mật khẩu sai:
```
[FAIL] Password verification failed: wrong password
```

### Hai bản ghi `salt:hash` của cùng một mật khẩu có giống nhau không?
**KHÔNG giống nhau** - Cùng mật khẩu "fit4012-demo-password" nhưng với salt khác nhau tạo ra hai bản ghi hoàn toàn khác nhau:
```
[PASS] Same password produced different salted records
```

## 4. Kết luận / Conclusion

### SHA-256 giúp phát hiện thay đổi dữ liệu như thế nào?
SHA-256 tạo ra một giá trị hash 256-bit duy nhất cho mỗi tập dữ liệu. Chỉ cần một thay đổi nhỏ (1 bit) trong dữ liệu gốc cũng tạo ra một hash hoàn toàn khác (hiệu ứng avalanche). Do đó, bằng cách so sánh hash hiện tại với hash ban đầu, ta có thể phát hiện liệu dữ liệu đã bị thay đổi hay chưa.

### Vì sao cần salt khi lưu hash mật khẩu?
Salt là một giá trị ngẫu nhiên được thêm vào trước khi băm mật khẩu. Salt giúp:
- Ngăn chặn tấn công bằng rainbow tables (bảng hash预先 tính toán)
- Đảm bảo cùng một mật khẩu nhưng với salt khác nhau tạo ra hash khác nhau
- Ngăn việc hai người dùng có cùng mật khẩu bị lộ thông tin qua hash giống nhau

### Vì sao SHA-256 demo trong lab chưa nên dùng trực tiếp cho hệ thống xác thực thật?
- SHA-256 quá nhanh, dễ bị tấn công brute-force với phần cứng hiện đại
- Không có cơ chế làm chậm (key stretching) như các thuật toán chuyên dụng
- Nên sử dụng Argon2id, bcrypt, hoặc scrypt - các thuật toán được thiết kế riêng cho password hashing với khả năng điều chỉnh thời gian tính toán và bộ nhớ