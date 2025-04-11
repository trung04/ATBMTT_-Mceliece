#include <stdio.h>
#include <string.h>
#include <openssl/sha.h> // Thêm d? dùng SHA256

#define N 16
#define K 8
#define T 2

int G_prime[K][N];

// Ð?c khóa công khai G'
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
            if (c == '0' || c == '1') {
                G_prime[i][j] = c - '0';
            } else {
                j--; // B? qua ký t? không ph?i 0/1
            }
        }
    }
    fclose(fp);
    return 1;
}

// Nhân ma tr?n G v?i vector l?i e ? tính h?i ch?ng
void matrix_vector_multiply(int G[K][N], int e[N], int result[K]) {
    for (int i = 0; i < K; i++) {
        result[i] = 0;
        for (int j = 0; j < N; j++) {
            result[i] ^= (G[i][j] & e[j]);
        }
    }
}

// Tr?ng s? Hamming c?a vector e
int hamming_weight(int e[N]) {
    int weight = 0;
    for (int i = 0; i < N; i++) weight += e[i];
    return weight;
}

// Bam thông di?p và chuy?n thành h?i ch?ng k? v?ng (d?ng bit)
void hash_message_to_syndrome(const char *message, int s_expected[K]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)message, strlen(message), hash);

    for (int i = 0; i < K; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        s_expected[i] = (hash[byte_index] >> (7 - bit_index)) & 1;
    }
}

int main() {
    char message[256];
    int e[N];
    int syndrome[K];
    int s_expected[K];

    if (!read_public_key()) return 1;

    FILE *fp = fopen("signed_message.txt", "r");
    if (!fp) {
        printf("Khong mo duoc signed_message.txt\n");
        return 1;
    }

    // Ð?c thông di?p
    fgets(message, sizeof(message), fp);
    message[strcspn(message, "\n")] = 0;

    // Ð?c vector l?i e
    for (int i = 0; i < N; i++) {
        char c;
        fscanf(fp, "%c", &c);
        while (c != '0' && c != '1') fscanf(fp, "%c", &c);
        e[i] = c - '0';
    }
    fclose(fp);

    // Tính h?i ch?ng t? e
    matrix_vector_multiply(G_prime, e, syndrome);

    // Tính h?i ch?ng k? v?ng t? thông di?p dã bam
    hash_message_to_syndrome(message, s_expected);

    // So sánh h?i ch?ng
    int syndrome_match = 1;
    for (int i = 0; i < K; i++) {
        if (syndrome[i] != s_expected[i]) {
            syndrome_match = 0;
            break;
        }
    }

    // Ki?m tra tính h?p l?
    int valid = 1;
    if (hamming_weight(e) > T || !syndrome_match) valid = 0;

    // Debug: In ra h?i ch?ng và h?i ch?ng k? v?ng
    printf("Syndrome (calculated):        ");
    for (int i = 0; i < K; i++) printf("%d", syndrome[i]);
    printf("\n");

    printf("Expected Syndrome (from msg): ");
    for (int i = 0; i < K; i++) printf("%d", s_expected[i]);
    printf("\n");

    // In thông di?p và k?t qu?
    printf("Thong diep: %s\n", message);
    if (valid) {
        printf("Chu ky hop le! Thong diep toan ven va tu nguon tin cay.\n");
    } else {
        printf("Chu ky khong hop le! Thong diep co the bi thay doi hoac khong tu nguon tin cay.\n");
    }

    return 0;
}

