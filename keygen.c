#include <stdio.h>
#include <stdlib.h>

// Tham số McEliece
#define N 16
#define K 8
#define T 2

// Ma trận sinh G (giả lập mã Goppa)
int G[K][N] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}
};

// Ma trận S (giả lập khả nghịch, đơn vị)
int S[K][K] = {
    {1, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1}
};

// Ma trận P (giả lập hoán vị, đơn vị)
int P[N][N] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

// Ma trận G' = S * G * P
int G_prime[K][N];

void matrix_multiply_SG(int S[K][K], int G[K][N], int result[K][N]) {
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < K; k++) {
                result[i][j] ^= (S[i][k] & G[k][j]);
            }
        }
    }
}

void matrix_multiply_GP(int SG[K][N], int P[N][N], int result[K][N]) {
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] ^= (SG[i][k] & P[k][j]);
            }
        }
    }
}

void save_private_key() {
    FILE *fp = fopen("private_key.txt", "w");
    if (!fp) {
        printf("Khong the tao private_key.txt\n");
        return;
    }
    // Lưu G
    fprintf(fp, "G:\n");
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp, "%d", G[i][j]);
        }
        fprintf(fp, "\n");
    }
    // Lưu S
    fprintf(fp, "S:\n");
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            fprintf(fp, "%d", S[i][j]);
        }
        fprintf(fp, "\n");
    }
    // Lưu P
    fprintf(fp, "P:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp, "%d", P[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("Da luu private key vao private_key.txt\n");
}

void save_public_key() {
    FILE *fp = fopen("public_key.txt", "w");
    if (!fp) {
        printf("Khong the tao public_key.txt\n");
        return;
    }
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp, "%d", G_prime[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("Da luu public key vao public_key.txt\n");
}

int main() {
    int SG[K][N];
    // Tính S * G
    matrix_multiply_SG(S, G, SG);
    // Tính (S * G) * P = G'
    matrix_multiply_GP(SG, P, G_prime);
    // Lưu khóa
    save_private_key();
    save_public_key();
    return 0;
}
