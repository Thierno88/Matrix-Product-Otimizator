#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <immintrin.h>
#include "matrix_lib.h"

// Estrutura para armazenar argumentos para cada thread
typedef struct {
    float scalar;
    struct matrix *mat;
    int start_row;
    int end_row;
} scalar_mult_args;

typedef struct {
    struct matrix *matA;
    struct matrix *matB;
    struct matrix *matC;
    int start_row;
    int end_row;
} matrix_mult_args;

// Função auxiliar para multiplicação escalar
void *scalar_matrix_mult_thread(void *arguments) {
    scalar_mult_args *args = (scalar_mult_args *)arguments;
    __m256 scalar_vector = _mm256_set1_ps(args->scalar);
    
    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 0; j < args->mat->width; j += 8) {
            __m256 matrix_values = _mm256_load_ps(&args->mat->rows[i * args->mat->width + j]);
            __m256 result = _mm256_mul_ps(matrix_values, scalar_vector);
            _mm256_store_ps(&args->mat->rows[i * args->mat->width + j], result);
        }
    }

    pthread_exit(NULL);
}

// Função auxiliar para multiplicação de matrizes
void *matrix_matrix_mult_thread(void *arguments) {
    matrix_mult_args *args = (matrix_mult_args *)arguments;

    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 0; j < args->matC->width; j += 8) {
            __m256 c_values = _mm256_setzero_ps();  // Inicializa vetor de resultado com zeros

            for (int k = 0; k < args->matA->width; k++) {
                __m256 a_values = _mm256_set1_ps(args->matA->rows[i * args->matA->width + k]);
                __m256 b_values = _mm256_load_ps(&args->matB->rows[k * args->matB->width + j]);
                c_values = _mm256_fmadd_ps(a_values, b_values, c_values);  // c += a * b
            }

            _mm256_store_ps(&args->matC->rows[i * args->matC->width + j], c_values);
        }
    }

    pthread_exit(NULL);
}

// Função de multiplicação escalar com threads e AVX
int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
    int num_threads = 4;  // Definir número de threads
    pthread_t threads[num_threads];
    scalar_mult_args args[num_threads];

    int rows_per_thread = matrix->height / num_threads;
    for (long t = 0; t < num_threads; t++) {
        args[t].scalar = scalar_value;
        args[t].mat = matrix;
        args[t].start_row = t * rows_per_thread;
        args[t].end_row = (t == num_threads - 1) ? matrix->height : args[t].start_row + rows_per_thread;
        
        if (pthread_create(&threads[t], NULL, scalar_matrix_mult_thread, (void *)&args[t])) {
            printf("Erro ao criar thread %ld para multiplicação escalar\n", t);
            return 0;
        }
    }

    for (long t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    return 1; // Retornar sucesso
}

// Função de multiplicação de matrizes com threads e AVX
int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC) {
    if (matrixA->width != matrixB->height) {
        printf("Erro: as dimensões das matrizes não são compatíveis para multiplicação\n");
        return 0;
    }

    int num_threads = 4;  // Definir número de threads
    pthread_t threads[num_threads];
    matrix_mult_args args[num_threads];

    int rows_per_thread = matrixA->height / num_threads;
    for (long t = 0; t < num_threads; t++) {
        args[t].matA = matrixA;
        args[t].matB = matrixB;
        args[t].matC = matrixC;
        args[t].start_row = t * rows_per_thread;
        args[t].end_row = (t == num_threads - 1) ? matrixA->height : args[t].start_row + rows_per_thread;

        if (pthread_create(&threads[t], NULL, matrix_matrix_mult_thread, (void *)&args[t])) {
            printf("Erro ao criar thread %ld para multiplicação de matrizes\n", t);
            return 0;
        }
    }

    for (long t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    return 1; // Retornar sucesso
}
