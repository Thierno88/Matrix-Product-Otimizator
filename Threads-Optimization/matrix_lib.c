// #include <stdio.h>
// #include <stdlib.h>
// #include <immintrin.h>

// struct matrix {
//     unsigned long int height;
//     unsigned long int width;
//     float *rows;
// };

// // MULTIPLICACAO ESCALAR SEM INSTRUCAO VETORIAL
// // int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
// //     if (matrix == NULL || matrix->rows == NULL) {
// //         return 0;  
// //     }

// //     for (int i = 0; i < matrix->height; i++) {
// //         for (int j = 0; j < matrix->width; j++) {
// //             matrix->rows[i * matrix->width + j] *= scalar_value;  // Multiplica cada elemento pelo escalar
// //         }
// //     }
// //     return 1; 
// // }

// // MULTIPLICACAO ESCALAR COM INSTRUCAO VETORIAL
// int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
//     if (matrix == NULL || matrix->rows == NULL) {
//         return 0;  
//     }

//     __m256 scalar_vector = _mm256_set1_ps(scalar_value); // Cria um vetor com o valor escalar repetido

//     int total_elements = matrix->height * matrix->width;
//     int i = 0;

//     // Operação em blocos de 8 elementos usando AVX
//     for (; i <= total_elements - 8; i += 8) {
//         __m256 matrix_values = _mm256_load_ps(&matrix->rows[i]);  // Carrega 8 floats da matriz
//         __m256 result = _mm256_mul_ps(matrix_values, scalar_vector); // Multiplica os 8 floats pelo escalar
//         _mm256_store_ps(&matrix->rows[i], result);  // Armazena o resultado de volta na matriz
//     }

//     // Lida com os elementos restantes (caso o número de elementos não seja múltiplo de 8)
//     for (; i < total_elements; i++) {
//         matrix->rows[i] *= scalar_value;
//     }

//     return 1;
// }

// int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC) {
//     if (matrixA == NULL || matrixB == NULL || matrixC == NULL ||
//         matrixA->rows == NULL || matrixB->rows == NULL || matrixC->rows == NULL ||
//         matrixA->width != matrixB->height) {
//         return 0;  // Retorna 0 para indicar erro
//     }

//     if (matrixC->height != matrixA->height || matrixC->width != matrixB->width) {
//         return 0;  // Retorna 0 se as dimensões de C estiverem incorretas
//     }

//     float zero = 0.0f;

//     // Zera a matriz C  
//     for (int i = 0; i < matrixA->height; i++) {
//         for (int j = 0; j < matrixC->width; j+=8) {
//             __m256 zero_vector = _mm256_set1_ps(zero);
//             _mm256_store_ps(&matrixC->rows[i * matrixC->width + j], zero_vector);  
//         }

//         // Multiplicação otimizada usando AVX
//         for (int k = 0; k < matrixA->width; k++) {
            
//             __m256 a_value = _mm256_set1_ps(matrixA->rows[i * matrixA->width + k]);

//             for (int j = 0; j < matrixB->width; j += 8) {  // Processa blocos de 8 colunas
//                 __m256 b_values =  _mm256_load_ps(&matrixB->rows[k * matrixB->width + j]);  // 8 elementos de B
//                 __m256 c_values =  _mm256_load_ps(&matrixC->rows[i * matrixC->width + j]);  // 8 elementos de C
//                 __m256 result = _mm256_fmadd_ps(a_value, b_values, c_values);  // Fused multiply-add
//                 _mm256_store_ps(&matrixC->rows[i * matrixC->width + j], result);  // Armazena o resultado em C
//             }
//         }
//     }

//     return 1;
// }


// void imprime_matriz(struct matrix matrix){
//     for (int i = 0; i < matrix.height; i++) {
//         for (int j = 0; j < matrix.width; j++) {
//             printf("%.2f ", matrix.rows[i * matrix.width + j]);
//         }
//         printf("\n");
//     }
// }


///////////////////////////////////////////

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
    int num_threads = 8;  // Definir número de threads
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

    int num_threads = 8;  // Definir número de threads
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
