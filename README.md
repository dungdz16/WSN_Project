**Wireless Sensor Network using BLE MESH**

Để sử dụng được project, cần tải đủ và cài đặt các gói cấu hình của ESP-IDF theo [hướng dẫn sau](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html)

Sau khi đã cài đặt biến môi trường và các gói đầy đủ, mỗi thư mục tương ứng với một node trong mạng.

- Project
  - Provisioner
  - Friend + Relay
  - Low power node

Để nạp được chương trình, người dùng cần mở IDF PowerShell ![](images/idf%20powershell.PNG)

Sau đó chuyển tới đường dẫn của thư mục tương ứng với chức năng muốn sử dụng ![](images/flash%20example.PNG)
Ví dụ với chức năng Provisioner, người dùng nhập **idf.py flash monitor -p [COM PORT]** để đồng thời build, flash và quan sát được màn hình LOG của node.
Ngoài ra còn các option khác như:

- **idf.py build** 
- **idf.py flash -p [COM PORT]**
- **idf.py monitor -p [COM PORT]**

**Chúc các bạn thành công!!!**

