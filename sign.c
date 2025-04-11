#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define N 16
#define K 8
#define T 2

int G_prime[K][N];

// Ð?c G' t? public_key.txt
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
                j--; // B? qua ký t? không h?p l?
            }
        }
    }
    fclose(fp);
    return 1;
}

// Hàm nhân ma tr?n G' * e => syndrome
void matrix_vector_multiply(int G[K][N], int e[N], int result[K]) {
    for (int i = 0; i < K; i++) {
        result[i] = 0;
        for (int j = 0; j < N; j++) {
            result[i] ^= (G[i][j] & e[j]);
        }
    }
}

// Hàm tính SHA-256
void sha256(const char *msg, unsigned char *hash) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, msg, strlen(msg));
    SHA256_Final(hash, &sha256);
}

// L?y K bit d?u t? SHA-256 d? làm syndrome
void extract_syndrome(unsigned char *hash, int syndrome[K]) {
    for (int i = 0; i < K; i++) {
        syndrome[i] = (hash[i / 8] >> (7 - (i % 8))) & 1;
    }
}

// Sinh e sao cho G' * e = syndrome và weight = T
int create_signature(int syndrome[K], int e[N]) {
    int candidate[N], temp[K];

    for (int bits = 0; bits < (1 << N); bits++) {
        int weight = 0;
        for (int i = 0; i < N; i++) {
            candidate[i] = (bits >> i) & 1;
            if (candidate[i]) weight++;
        }
        if (weight > T) continue;

        matrix_vector_multiply(G_prime, candidate, temp);

        int match = 1;
        for (int i = 0; i < K; i++) {
            if (temp[i] != syndrome[i]) {
                match = 0;
                break;
            }
        }

        if (match) {
            memcpy(e, candidate, sizeof(int) * N);
            return 1;
        }
    }

    return 0; // Không tìm du?c e phù h?p
}

int main() {
    char message[256];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    int syndrome[K];
    int e[N];

    if (!read_public_key()) return 1;

    printf("Nhap thong diep: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;

    // Tính hash và l?y syndrome
    sha256(message, hash);
    extract_syndrome(hash, syndrome);

    // T?o ch? ký (e)
    if (!create_signature(syndrome, e)) {
        printf("Khong tim duoc vector e phu hop voi syndrome.\n");
        return 1;
    }

    FILE *fp = fopen("signed_message.txt", "w");
    if (!fp) {
        printf("Khong the tao signed_message.txt\n");
        return 1;
    }
    fprintf(fp, "%s\n", message);
    for (int i = 0; i < N; i++) fprintf(fp, "%d", e[i]);
    fprintf(fp, "\n");
    fclose(fp);

    printf("Da ky thong diep va luu vao signed_message.txt\n");
    return 0;
}

