# 1. Chọn nền tảng: Sử dụng môi trường có sẵn trình biên dịch GCC (để dịch C++)
FROM gcc:latest

# 2. Tạo thư mục làm việc: Tạo một thư mục tên là /app trong Docker để chứa code
WORKDIR /app

# 3. Copy nguyên liệu: Copy toàn bộ file từ máy tính của bạn vào thư mục /app trong Docker
COPY . .

# 4. Chế biến (Biên dịch): Dùng g++ để dịch file main.cpp thành file chạy tên là myapp
# Lưu ý: Code của bạn cần file users.txt để chạy, lệnh COPY ở trên đã lo việc đó rồi.
RUN g++ -o myapp main.cpp

# 5. Thưởng thức (Chạy): Khi container khởi động, nó sẽ chạy file myapp
CMD ["./myapp"]