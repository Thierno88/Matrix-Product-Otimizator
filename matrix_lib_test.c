// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// #include <string.h>
// #include <errno.h>
// #include "timer.h"
// #include "matrix_lib.h"

// float scalar_value = 0.0f;

// struct matrix matrixA, matrixB, matrixC;

// int store_matrix(struct matrix *matrix, char *filename) {
//     FILE *file = fopen(filename, "wb");
//     if (file == NULL) {
//         printf("Erro ao abrir o arquivo %s\n", filename);
//         return 0;
//     }

//     fwrite(matrix->rows, sizeof(float), matrix->height * matrix->width, file);
//     fclose(file);
//     return 1;
// }

// int load_matrix(struct matrix *matrix, char *filename) {
//     FILE *file = fopen(filename, "rb");
//     if (file == NULL) {
//         printf("Erro ao abrir o arquivo %s\n", filename);
//         return 0;
//     }

//     fread(matrix->rows, sizeof(float), matrix->height * matrix->width, file);
//     fclose(file);
//     return 1;
// }


// int initialize_matrix(struct matrix *matrix, float value, float inc) {
//     if (!matrix || !matrix->rows) {
//         return 0;
//     }

//     for (int i = 0; i < matrix->height; i++) {
//         for (int j = 0; j < matrix->width; j++) {
//             matrix->rows[i * matrix->width + j] = value;
//             value += inc;
//         }
//     }

//     return 1;
// }

// int print_matrix(struct matrix *matrix) {
//     for (unsigned long i = 0; i < matrix->height; i++) {
//         for (unsigned long j = 0; j < matrix->width; j++) {
//             printf("%.2f ", matrix->rows[i * matrix->width + j]);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

// // int check_errors(struct matrix *matrix, float scalar_value) {
// //     if (!matrix || !matrix->rows) {
// //         return 0;
// //     }

// //     int error_count = 0;
// //     for (int i = 0; i < matrix->height; i++) {
// //         for (int j = 0; j < matrix->width; j++) {
// //             float value = matrix->rows[i * matrix->width + j];
// //             if (fabs(value - scalar_value) > 0.001) {
// //                 printf("Erro na posição (%d, %d): Valor = %f, Esperado = %f\n", i, j, value, scalar_value);
// //                 error_count++;
// //             }
// //         }
// //     }

// //     return error_count == 0 ? 1 : 0;
// // }

// int main(int argc, char *argv[]) {
//   unsigned long int DimA_M, DimA_N, DimB_M, DimB_N;
//   char *matrixA_filename, *matrixB_filename, *result1_filename, *result2_filename;
//   char *eptr = NULL;
//   struct timeval start, stop, overall_t1, overall_t2;

//   // Mark overall start time
//   gettimeofday(&overall_t1, NULL);

//   // Check arguments
//   if (argc != 10) {
//         printf("Usage: %s <scalar_value> <DimA_M> <DimA_N> <DimB_M> <DimB_N> <matrixA_filename> <matrixB_filename> <result1_filename> <result2_filename>\n", argv[0]);
//         return 0;
//   }

//     // Convertendo argumentos 
//     float scalar = atof(argv[1]);
//     unsigned long heightA = strtoul(argv[2], NULL, 10);
//     unsigned long widthA = strtoul(argv[3], NULL, 10);
//     unsigned long heightB = strtoul(argv[4], NULL, 10);
//     unsigned long widthB = strtoul(argv[5], NULL, 10);

//     const char *fileA = argv[6];  // Primeiro arquivo binário para matriz A
//     const char *fileB = argv[7];  // Segundo arquivo binário para matriz B
//     const char *fileC = argv[8];  // Terceiro arquivo binário para matriz resultado de multiplicação escalar
//     const char *fileD = argv[9];  // Quarto arquivo binário para matriz resultado da multiplicação matriz-matriz

//     // Inicializando as matrizes
//     struct matrix matrixA, matrixB, matrixC;

//     matrixA.height = heightA;
//     matrixA.width = widthA;
//     matrixA.rows = (float*)malloc(matrixA.height * matrixA.width * sizeof(float));

//     matrixB.height = heightB;
//     matrixB.width = widthB;
//     matrixB.rows = (float*)malloc(matrixB.height * matrixB.width * sizeof(float));

//     matrixC.height = matrixA.height;
//     matrixC.width = matrixB.width;
//     matrixC.rows = (float*)malloc(matrixC.height * matrixC.width * sizeof(float));

//     if (matrixA.rows == NULL || matrixB.rows == NULL || matrixC.rows == NULL) {
//         printf("Erro ao alocar memória para as matrizes\n");
//         return 1;
//     }

//     // Lendo matrizes dos arquivos binários
//     if (!load_matrix(fileA, &matrixA) || !load_matrix(fileB, &matrixB)) {
//         printf("Erro ao ler matrizes dos arquivos binários\n");
//         free(matrixA.rows);
//         free(matrixB.rows);
//         free(matrixC.rows);
//         return 1;
//     }

//   /* Scalar product of matrix A */
//   printf("Executing scalar_matrix_mult(%5.1f, matrixA)...\n",scalar_value);
//   gettimeofday(&start, NULL);
//   if (!scalar_matrix_mult(scalar_value, &matrixA)) {
// 	printf("%s: scalar_matrix_mult problem.", argv[0]);
// 	return 1;
//   }
//   gettimeofday(&stop, NULL);
//   printf("%f ms\n", timedifference_msec(start, stop));

//   /* Print matrix */
//   printf("---------- Matrix A ----------\n");
//   print_matrix(&matrixA);

//   /* Write first result */
//   printf("Writing first result: %s...\n", result1_filename);
//   if (!store_matrix(&matrixA, result1_filename)) {
// 	printf("%s: failed to write first result to file.", argv[0]);
// 	return 1;
//   }

//   /* Calculate the product between matrix A and matrix B */
//   printf("Executing matrix_matrix_mult(matrixA, mattrixB, matrixC)...\n");
//   gettimeofday(&start, NULL);
//   if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
// 	printf("%s: matrix_matrix_mult problem.", argv[0]);
// 	return 1;
//   }
//   gettimeofday(&stop, NULL);
//   printf("%f ms\n", timedifference_msec(start, stop));

//   /* Print matrix */
//   printf("---------- Matrix C ----------\n");
//   print_matrix(&matrixC);

//   /* Write second result */
//   printf("Writing second result: %s...\n", result2_filename);
//   if (!store_matrix(&matrixC, result2_filename)) {
// 	printf("%s: failed to write second result to file.", argv[0]);
// 	return 1;
//   }

//   /* Check foor errors */
// //   printf("Checking matrixC for errors...\n");
// //   gettimeofday(&start, NULL);
// //   check_errors(&matrixC, 10240.0f);
// //   gettimeofday(&stop, NULL);
// //   printf("%f ms\n", timedifference_msec(start, stop));

//   // Mark overall stop time
//   gettimeofday(&overall_t2, NULL);

//   // Show elapsed overall time
//   printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));

//   return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix_lib.h"  
#include <math.h>
#include <errno.h>
#include "timer.h"

// Função auxiliar para ler uma matriz de um arquivo binário
int read_matrix_from_file(const char* filename, struct matrix* mat) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return 0;
    }

    fread(mat->rows, sizeof(float), mat->height * mat->width, file);
    fclose(file);
    return 1;
}

// Função auxiliar para escrever uma matriz em um arquivo binário
int write_matrix_to_file(const char* filename, struct matrix* mat) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return 0;
    }

    fwrite(mat->rows, sizeof(float), mat->height * mat->width, file);
    fclose(file);
    return 1;
}

void print_matrix(struct matrix* mat) {
    for (unsigned long i = 0; i < mat->height; i++) {
        for (unsigned long j = 0; j < mat->width; j++) {
            printf("%.2f ", mat->rows[i * mat->width + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {

    struct matrix matrixA, matrixB, matrixC;
    struct timeval start, stop, overall_t1, overall_t2;
    gettimeofday(&overall_t1, NULL);

    if (argc != 10) {
        printf("Uso: %s <valor escalar> <altura matriz A> <largura matriz A> <altura matriz B> <largura matriz B> <arquivos>\n", argv[0]);
        return 1;
    }

    float scalar = atof(argv[1]);
    unsigned long heightA = strtoul(argv[2], NULL, 10);
    unsigned long widthA = strtoul(argv[3], NULL, 10);
    unsigned long heightB = strtoul(argv[4], NULL, 10);
    unsigned long widthB = strtoul(argv[5], NULL, 10);

    const char *fileA = argv[6];  // Primeiro arquivo binário para matriz A
    const char *fileB = argv[7];  // Segundo arquivo binário para matriz B
    const char *fileC = argv[8];  // Terceiro arquivo binário para matriz resultado de multiplicação escalar
    const char *fileD = argv[9];  // Quarto arquivo binário para matriz resultado da multiplicação matriz-matriz

    // Alocando as matrizes
    matrixA.height = heightA;
    matrixA.width = widthA;
    matrixA.rows = (float*)malloc(matrixA.height * matrixA.width * sizeof(float));

    matrixB.height = heightB;
    matrixB.width = widthB;
    matrixB.rows = (float*)malloc(matrixB.height * matrixB.width * sizeof(float));

    matrixC.height = matrixA.height;
    matrixC.width = matrixB.width;
    matrixC.rows = (float*)malloc(matrixC.height * matrixC.width * sizeof(float));

    if (matrixA.rows == NULL || matrixB.rows == NULL || matrixC.rows == NULL) {
        printf("Erro ao alocar memória para as matrizes\n");
        return 1;
    }

    // Lendo matrizes dos arquivos binários
    if (!read_matrix_from_file(fileA, &matrixA) || !read_matrix_from_file(fileB, &matrixB)) {
        printf("Erro ao ler matrizes dos arquivos binários\n");
        free(matrixA.rows);
        free(matrixB.rows);
        free(matrixC.rows);
        return 1;
    }

    // Multiplicação escalar
    printf("Executing Scalar por 5.0.\n",scalar);
    gettimeofday(&start, NULL);
    if (!scalar_matrix_mult(scalar, &matrixA)) {
        printf("%s: scalar_matrix_mult problem.", argv[0]);
        return 1;
     }
    gettimeofday(&stop, NULL);
    printf("Tempo de execucao: %f ms\n", timedifference_msec(start, stop));


    // Escrevendo o resultado da multiplicação escalar em um arquivo binário
    if (!write_matrix_to_file(fileC, &matrixA)) {
        printf("Erro ao escrever matriz no arquivo binário\n");
        free(matrixA.rows);
        free(matrixB.rows);
        free(matrixC.rows);
        return 1;
    }


  printf("Multiplicação de matrizes\n");
  gettimeofday(&start, NULL);
  if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
	printf("%s: matrix_matrix_mult problem.", argv[0]);
	return 1;
  }
  gettimeofday(&stop, NULL);
  printf("Tempo de Execucao: %f ms\n", timedifference_msec(start, stop));

    if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
        printf("Erro na multiplicação de matrizes\n");
        free(matrixA.rows);
        free(matrixB.rows);
        free(matrixC.rows);
        return 1;
    }

    // Escrevendo o resultado da multiplicação matriz-matriz em um arquivo binário
    if (!write_matrix_to_file(fileD, &matrixC)) {
        printf("Erro ao escrever matriz no arquivo binário\n");
        free(matrixA.rows);
        free(matrixB.rows);
        free(matrixC.rows);
        return 1;
    }

    printf("Operações concluídas com sucesso!\n");


    // printf("Imprimindo matrix A:\n");
    // print_matrix(&matrixA);

    // printf("-------------------------------\n");
    // printf("Imprimindo matrix B:\n");
    // print_matrix(&matrixB);

    //printf("Imprimindo matrix C:\n");
    // print_matrix(&matrixC);

    // // Liberando memória
    // free(matrixA.rows);
    // free(matrixB.rows);
    // free(matrixC.rows);

    printf("Elemento na posição (10, 10) da matriz A: %.2f\n", matrixA.rows[9 * matrixA.width + 9]);
    printf("Elemento na posição (10, 10) da matriz B: %.2f\n", matrixB.rows[9 * matrixB.width + 9]);
    printf("Elemento na posição (10, 10) da matriz C: %.2f\n", matrixC.rows[9 * matrixC.width + 9]);

    return 0;
}