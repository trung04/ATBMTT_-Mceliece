#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Added for time()

#define N 6 // Code length
#define K 3 // Message length
#define T 1 // Error correction capability

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

// Generate invertible matrix S (identity for simplicity)
void generate_invertible_matrix(int s[K][K]) {
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            s[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// Generate permutation matrix P (identity for simplicity)
void generate_permutation_matrix(int p[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            p[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// Generate inverse permutation matrix P^-1
void generate_inverse_permutation_matrix(int p_inv[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            p_inv[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// Generate inverse of S
void inverse_matrix_S(int s[K][K], int s_inv[K][K]) {
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            s_inv[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// Improved error correction for Goppa code
void correct_errors(int c_prime[N], int mS[K], int e_prime[N], int G[K][N]) {
    // Initialize error vector
    for (int i = 0; i < N; i++) e_prime[i] = 0;

    // Try all possible messages (for K=3, there are 2^3=8 possibilities)
    int min_errors = N + 1;
    int best_mS[K] = {0};
    int possible_m[K];
    int codeword[N];

    for (int m0 = 0; m0 <= 1; m0++) {
        for (int m1 = 0; m1 <= 1; m1++) {
            for (int m2 = 0; m2 <= 1; m2++) {
                possible_m[0] = m0;
                possible_m[1] = m1;
                possible_m[2] = m2;

                // Compute codeword = m * G
                matrix_multiply(1, K, &possible_m, K, N, G, &codeword);

                // Compute Hamming distance between c_prime and codeword
                int errors = 0;
                for (int i = 0; i < N; i++) {
                    if (c_prime[i] != codeword[i]) errors++;
                }

                // Update best mS if this has fewer errors
                if (errors < min_errors) {
                    min_errors = errors;
                    for (int i = 0; i < K; i++) {
                        best_mS[i] = possible_m[i];
                    }
                    // Compute error vector
                    for (int i = 0; i < N; i++) {
                        e_prime[i] = c_prime[i] ^ codeword[i];
                    }
                }
            }
        }
    }

    // Copy best mS to output
    for (int i = 0; i < K; i++) {
        mS[i] = best_mS[i];
    }
}

int main(int argc, char *argv[]) {
    // Fixed G matrix (simulating Goppa code)
    int G[K][N] = {
        {1, 0, 0, 1, 1, 0},
        {0, 1, 0, 0, 1, 1},
        {0, 0, 1, 1, 0, 1}
    };
    int S[K][K], P[N][N], G_prime[K][N];

    if (argc == 1) {
        // Key generation mode
        srand(time(NULL));

        generate_invertible_matrix(S);
        generate_permutation_matrix(P);

        // Compute G' = S * G * P
        int SG[K][N];
        matrix_multiply(K, K, S, K, N, G, SG);
        matrix_multiply(K, N, SG, N, N, P, G_prime);

        // Save public key
        FILE *fp = fopen("public_key.txt", "w");
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(fp, "%d ", G_prime[i][j]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);

        // Save secret key
        fp = fopen("secret_key.txt", "w");
        fprintf(fp, "G:\n");
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(fp, "%d ", G[i][j]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "S:\n");
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < K; j++) {
                fprintf(fp, "%d ", S[i][j]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "P:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(fp, "%d ", P[i][j]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);

        printf("Generated public key and secret key.\n");
        printf("Sending public key to Bob...\n");
        system("scp public_key.txt trung@192.168.198.132:/home/trung/Desktop");

        return 0;
    } else if (argc == 2 && strcmp(argv[1], "decrypt") == 0) {
        // Decryption mode
        // Read secret key
        int G_read[K][N], S_read[K][K], P_read[N][N];
        FILE *fp = fopen("secret_key.txt", "r");
        char buffer[256];
        fscanf(fp, "%s", buffer); // "G:"
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < N; j++) {
                fscanf(fp, "%d", &G_read[i][j]);
            }
        }
        fscanf(fp, "%s", buffer); // "S:"
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < K; j++) {
                fscanf(fp, "%d", &S_read[i][j]);
            }
        }
        fscanf(fp, "%s", buffer); // "P:"
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fscanf(fp, "%d", &P_read[i][j]);
            }
        }
        fclose(fp);

        // Read ciphertext
        int c[N];
        fp = fopen("ciphertext.txt", "r");
        if (fp == NULL) {
            printf("Error: Cannot open ciphertext.txt\n");
            return 1;
        }
        for (int i = 0; i < N; i++) {
            fscanf(fp, "%d", &c[i]);
        }
        fclose(fp);

        printf("Received ciphertext: ");
        for (int i = 0; i < N; i++) printf("%d ", c[i]);
        printf("\n");

        // Compute c' = c * P^-1
        int P_inv[N][N];
        generate_inverse_permutation_matrix(P_inv);
        int c_prime[N];
        matrix_multiply(1, N, &c, N, N, P_inv, &c_prime);

        printf("c' after permutation: ");
        for (int i = 0; i < N; i++) printf("%d ", c_prime[i]);
        printf("\n");

        // Correct errors
        int mS[K], e_prime[N];
        correct_errors(c_prime, mS, e_prime, G_read);

        printf("m * S: ");
        for (int i = 0; i < K; i++) printf("%d ", mS[i]);
        printf("\n");

        // Compute m = mS * S^-1
        int S_inv[K][K];
        inverse_matrix_S(S_read, S_inv);
        int m[K];
        matrix_multiply(1, K, &mS, K, K, S_inv, &m);

        printf("Decrypted message: ");
        for (int i = 0; i < K; i++) printf("%d ", m[i]);
        printf("\n");

        // Verify correctness
        int expected_m[K] = {1, 0, 1};
        int correct = 1;
        for (int i = 0; i < K; i++) {
            if (m[i] != expected_m[i]) {
                correct = 0;
                break;
            }
        }
        printf("Verification: %s\n", correct ? "Correct" : "Incorrect");

        return 0;
    }

    printf("Usage: ./alice [decrypt]\n");
    return 1;
}
