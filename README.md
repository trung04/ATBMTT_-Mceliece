# McEliece Digital Signature Simulation

Mô phỏng quá trình tạo và xác minh chữ ký số sử dụng McEliece CFS giữa hai máy (Sender và Recipient).

---

## 📌 Cấu trúc dự án

### 🔐 Bên A (Sender):
1. **`keygen.c`** – Sinh cặp khóa (private key & public key).
2. **`sign.c`** – Tạo thông điệp và thực hiện ký số.

### ✅ Bên B (Recipient):
1. **`verifier.c`** – Xác minh chữ ký và kiểm tra thông điệp nhận được.

---
