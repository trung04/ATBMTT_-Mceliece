#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define K 3
#define T 1

// Matrix multiplication (binary)
void matrix_multiply(int rows_a, int cols_a, int a[rows_a][cols_a], 
                     int rows_b, int cols_b, int b[rows_b][cols_b], 
                     int result[rows_a][cols_b]) {
    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < cols_b; j++) {
            result[i][j] = 0;
            for (int k = 0; k < cols_a; k++) {
                result[i][j] ^= (a[i][k] & b[k][j]);
            }
        }
    }
}

// Generate error vector with exactly T errors
void generate_error_vector(int e[N], int t) {
    for (int i = 0; i < N; i++) e[i] = 0;
    // For reproducibility, set error at fixed position (e.g., index 2)
    e[2] = 1; // Single error at position 2
}

int main() {
    srand(time(NULL));

    // Read public key
    int G_prime[K][N];
    FILE *fp = fopen("public_key.txt", "r");
    if (fp == NULL) {
        printf("Error: Cannot open public_key.txt\n");
        return 1;
    }
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(fp, "%d", &G_prime[i][j]);
        }
    }
    fclose(fp);

    printf("Received public key G':\n");
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", G_prime[i][j]);
        }
        printf("\n");
    }

    // Message
    int m[K] = {1, 0, 1};
    printf("Message to encrypt: ");
    for (int i = 0; i < K; i++) printf("%d ", m[i]);
    printf("\n");

    // Compute m * G'
    int mG[K][N];
    matrix_multiply(1, K, &m, K, N, G_prime, mG);

    // Generate error vector
    int e[N];
    generate_error_vector(e, T);
    printf("Error vector e: ");
    for (int i = 0; i < N; i++) printf("%d ", e[i]);
    printf("\n");

    // Compute c = m * G' + e
    int c[N];
    for (int i = 0; i < N; i++) {
        c[i] = mG[0][i] ^ e[i];
    }

    printf("Ciphertext c: ");
    for (int i = 0; i < N; i++) printf("%d ", c[i]);
    printf("\n");

    // Save ciphertext
    fp = fopen("ciphertext.txt", "w");
    for (int i = 0; i < N; i++) fprintf(fp, "%d ", c[i]);
    fclose(fp);

    // Send to Alice
    printf("Sending ciphertext to Alice...\n");
    system("scp public_key.txt trung@192.168.198.130:/home/trung/Desktop");

    return 0;
}
