# --- Biến (Variables) ---
# Trình biên dịch C
CC = gcc
# Các cờ biên dịch:
# -Wall: Bật tất cả các cảnh báo quan trọng
# -Wextra: Bật thêm một số cảnh báo khác
# -g: Thêm thông tin gỡ lỗi (debug) vào file thực thi
# -std=c99: Sử dụng chuẩn C99
CFLAGS = -Wall -Wextra -g -std=c99

# Tên của file thực thi cuối cùng
TARGET = bai1

# --- Quy tắc (Rules) ---

# .PHONY nói cho make biết rằng "all" và "clean" không phải là tên của file thật
# Chúng chỉ là tên của các hành động.
.PHONY: all clean

# Quy tắc "all" là quy tắc mặc định. Nó sẽ build TARGET.
all: $(TARGET)

# Quy tắc để liên kết (link) các file object thành file thực thi
$(TARGET): bai1.o
	$(CC) $(CFLAGS) bai1.o -o $(TARGET)

# Quy tắc để biên dịch file source .c thành file object .o
bai1.o: bai1.c
	$(CC) $(CFLAGS) -c bai1.c -o bai1.o

# Quy tắc để "dọn dẹp" thư mục
clean:
	# Lệnh del dùng cho Windows, rm -f dùng cho Linux/macOS
	# Bạn có thể giữ cả hai hoặc chỉ giữ lệnh cho hệ điều hành của mình
	rm -f *.o $(TARGET)
