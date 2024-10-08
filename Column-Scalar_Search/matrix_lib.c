#include <stdio.h>
#include <stdlib.h>

struct matrix {
    unsigned long int height;
    unsigned long int width;
    float *rows;
};

int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
    if (matrix == NULL || matrix->rows == NULL) {
        return 0;  
    }

    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            matrix->rows[i * matrix->width + j] *= scalar_value;  // Multiplica cada elemento pelo escalar
        }
    }
    return 1; 
}

// NAO OTIMIZADO
// int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC) {

//     if (matrixA == NULL || matrixB == NULL || matrixC == NULL ||
//         matrixA->rows == NULL || matrixB->rows == NULL || matrixC->rows == NULL ||
//         matrixA->width != matrixB->height) {
//         return 0;  // Retorna 0 para indicar erro
//     }

//     if (matrixC->height != matrixA->height || matrixC->width != matrixB->width) {
//         return 0;  // Retorna 0 se as dimensões de C estiverem incorretas
//     }

//     for (int i = 0; i < matrixA->height; i++) {
//         for (int j = 0; j < matrixB->width; j++) {
//             matrixC->rows[i * matrixC->width + j] = 0;  
//             for (int k = 0; k < matrixA->width; k++) {
//                 matrixC->rows[i * matrixC->width + j] += matrixA->rows[i * matrixA->width + k] * matrixB->rows[k * matrixB->width + j];
//             }
//         }
//     }

//     return 1;  // Retorna 1 para indicar sucesso
// }

// ALGORITMO OTIMIZADO FUNCIONANDO
int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC) {

    // Verificações de integridade das matrizes
    if (matrixA == NULL || matrixB == NULL || matrixC == NULL ||
        matrixA->rows == NULL || matrixB->rows == NULL || matrixC->rows == NULL ||
        matrixA->width != matrixB->height) {
        return 0;  // Retorna 0 para indicar erro
    }

    if (matrixC->height != matrixA->height || matrixC->width != matrixB->width) {
        return 0;  // Retorna 0 se as dimensões de C estiverem incorretas
    }

    // Passo 1 e 2: Para cada linha i de A, inicialize a linha i de C com zeros
    for (int i = 0; i < matrixA->height; i++) {
        for (int j = 0; j < matrixC->width; j++) {
            matrixC->rows[i * matrixC->width + j] = 0;  // Inicializa com zeros
        }

        // Passo 3: Para cada elemento ai1, ai2, ..., ain da linha i de A
        for (int k = 0; k < matrixA->width; k++) {
            // Multiplicando o elemento ai[k] pelo elemento correspondente em B
            for (int j = 0; j < matrixB->width; j++) {
                matrixC->rows[i * matrixC->width + j] += matrixA->rows[i * matrixA->width + k] * matrixB->rows[k * matrixB->width + j];
            }
        }
    }

    return 1;  // Retorna 1 para indicar sucesso
}

void imprime_matriz(struct matrix matrix){
    for (int i = 0; i < matrix.height; i++) {
        for (int j = 0; j < matrix.width; j++) {
            printf("%.2f ", matrix.rows[i * matrix.width + j]);
        }
        printf("\n");
    }
}
