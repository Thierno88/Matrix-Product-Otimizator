
#include <stdio.h>
#include <stdlib.h>

// Definição da estrutura de matriz
struct matrix {
    unsigned long int height; // Número de linhas
    unsigned long int width;  // Número de colunas
    float *rows;              // Ponteiro para os elementos da matriz
};

// Protótipos das funções

//Multiplica todos os elementos da matriz por um valor escalar.
int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

//Multiplica duas matrizes A e B e armazena o resultado na matriz C.
int matrix_matrix_mult_otimizado(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC);

//Imprime uma Matriz
void imprime_matriz(struct matrix matrix);

