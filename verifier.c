#include <stdio.h>
#include <string.h>

#define N 16
#define K 8
#define T 2

int G_prime[K][N];

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
                j--; // Bỏ qua khoảng trắng hoặc xuống dòng
            }
        }
    }
    fclose(fp);
    return 1;
}

void matrix_vector_multiply(int G[K][N], int e[N], int result[K]) {
    for (int i = 0; i < K; i++) {
        result[i] = 0;
        for (int j = 0; j < N; j++) {
            result[i] ^= (G[i][j] & e[j]);
        }
    }
}

int hamming_weight(int e[N]) {
    int weight = 0;
    for (int i = 0; i < N; i++) weight += e[i];
    return weight;
}

int main() {
    char message[256];
    int e[N];
    int syndrome[K];

    if (!read_public_key()) return 1;

    FILE *fp = fopen("signed_message.txt", "r");
    if (!fp) {
        printf("Khong mo duoc signed_message.txt\n");
        return 1;
    }

    fgets(message, sizeof(message), fp);
    message[strcspn(message, "\n")] = 0;

    for (int i = 0; i < N; i++) {
        char c;
        fscanf(fp, "%c", &c);
        while (c != '0' && c != '1') fscanf(fp, "%c", &c);
        e[i] = c - '0';
    }
    fclose(fp);

    matrix_vector_multiply(G_prime, e, syndrome);


    int valid = 1;
    if (hamming_weight(e) > T) valid = 0;

    // Debug: in syndrome
    printf("Syndrome: ");
    for (int i = 0; i < K; i++) printf("%d", syndrome[i]);
    printf("\n");

    printf("Thong diep: %s\n", message);
    if (valid) {
        printf("Chu ky hop le! Thong diep toan ven va tu nguon tin cay.\n");
    } else {
        printf("Chu ky khong hop le! Thong diep co the bi thay doi hoac khong tu nguon tin cay.\n");
    }

    return 0;
}

