# STM32 Lab 3 — Buttons and Switches

Lab 3 thực hành hệ thống đèn giao thông bốn hướng với STM32F103C6, bốn LED 7 đoạn và ba nút MODE, INC, SET. Mã nguồn dùng STM32CubeIDE/HAL; mạch mô phỏng dùng Proteus.

## Nội dung repo

- `Ex1/`: sơ đồ FSM và phần mô tả bài 1.
- `proteus/Lab3_Ex2.pdsprj`: mạch Proteus được mở rộng qua các bài.
- `Lab3_Ex3/`: project STM32CubeIDE tích lũy qua các bài 3–10; tên project gốc được giữ để file `.ioc` và cấu hình IDE khớp.
- `Lab3_Ex3/Debug/Lab3_Ex3.hex`: firmware build gần nhất dùng cho mô phỏng.

Các bài 3–10 được phát triển liên tiếp trong cùng một project, nên repo lưu phiên bản tích hợp cuối cùng, không có bản mã nguồn riêng cho từng bài.

## Chạy mô phỏng

1. Mở `Lab3_Ex3/Lab3_Ex3.ioc` hoặc import thư mục `Lab3_Ex3` vào STM32CubeIDE để xem/cấu hình và build mã nguồn.
2. Mở `proteus/Lab3_Ex2.pdsprj` trong Proteus.
3. Ở thuộc tính **Program File** của STM32, chọn `Lab3_Ex3/Debug/Lab3_Ex3.hex` từ repo này nếu đường dẫn cũ trên máy không còn đúng.
4. Chạy mô phỏng. Nút MODE chọn chế độ 1–4; trong chế độ chỉnh sửa, INC tăng giá trị và SET lưu giá trị.

Mặc định: đỏ 5 giây, vàng 2 giây, xanh 3 giây. TIM2 tạo ngắt mỗi 10 ms để đọc và chống dội nút. LED 7 đoạn được quét luân phiên trong vòng lặp chính.

**Lưu ý:** Giá trị chỉnh sửa được giữ trong RAM và trở về mặc định sau khi reset. Code hiện tại cho phép chỉnh riêng ba khoảng thời gian 1–99 giây nhưng chưa kiểm tra quan hệ giữa các khoảng; khi demo các giá trị khác mặc định, nên chọn thời gian đỏ bằng tổng thời gian xanh và vàng để hai hướng hoạt động nhất quán.
