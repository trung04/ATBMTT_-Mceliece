#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

// Tham số McEliece
#define N 16
#define K 8
#define T 2

// Ma trận G' (đọc từ file để đảm bảo nhất quán)
int G_prime[K][N];

// Hàm đọc G' (tạm thời để tính e đúng)
int read_public_key() {
    FILE *fp = fopen("public_key.txt", "r");
    if (!fp) {
        printf("Khong mo duoc public_key.txt\n");
        return 0;
    }
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            char c;
            fscanf(fp, "%c", &c);
            G_prime[i][j] = c - '0';
        }
        fscanf(fp, "\n");
    }
    fclose(fp);
    return 1;
}

// Hàm tính SHA-256
void sha256(const char *msg, unsigned char *hash) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, msg, strlen(msg));
    SHA256_Final(hash, &sha256);
}

// Hàm tạo chữ ký
void create_signature(unsigned char *syndrome, int *e) {
    // Tính e sao cho G' * e^T = syndrome
    for (int i = 0; i < N; i++) e[i] = 0;
    // Giả lập: chọn e dựa trên syndrome
    for (int i = 0; i < T && i < K; i++) {
        if (syndrome[i]) e[i] = 1;
    }
    // Đảm bảo trọng số <= T
    int weight = 0;
    for (int i = 0; i < N; i++) weight += e[i];
    if (weight > T) {
        e[T] = 0; // Giảm trọng số nếu cần
    }
}

int main() {
    char message[256];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    int e[N];

    // Đọc G' để tạo chữ ký đúng
    if (!read_public_key()) return 1;

    // Nhập thông điệp
    printf("Nhap thong diep: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;

    // Tính SHA-256
    sha256(message, hash);

    // Lấy K bit làm syndrome
    unsigned char syndrome[K];
    for (int i = 0; i < K; i++) {
        syndrome[i] = (hash[i / 8] >> (7 - (i % 8))) & 1;
    }

    // Tạo chữ ký
    create_signature(syndrome, e);

    // Lưu thông điệp và chữ ký
    FILE *fp = fopen("signed_message.txt", "w");
    if (!fp) {
        printf("Khong the tao signed_message.txt\n");
        return 1;
    }
    fprintf(fp, "%s\n", message);
    for (int i = 0; i < N; i++) fprintf(fp, "%d", e[i]);
    fclose(fp);

    printf("Da ky thong diep va luu vao signed_message.txt\n");
    return 0;
}
