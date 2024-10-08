#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix_lib.h"
#include <math.h>
#include <errno.h>
#include "timer.h"

// Função auxiliar para ler uma matriz de um arquivo binário
int
read_matrix_from_file (const char *filename, struct matrix *mat)
{
    FILE * file = fopen (filename, "rb");
    if (file == NULL)
    {
        printf ("Erro ao abrir o arquivo %s\n", filename);
        return 0;
    }
    fread (mat->rows, sizeof (float), mat->height * mat->width, file);
    fclose (file);
    return 1;
}




// Função auxiliar para escrever uma matriz em um arquivo binário
int
write_matrix_to_file (const char *filename, struct matrix *mat)
{
    FILE * file = fopen (filename, "wb");
    if (file == NULL)
    {
        printf ("Erro ao abrir o arquivo %s\n", filename);
        return 0;
    }
    fwrite (mat->rows, sizeof (float), mat->height * mat->width, file);
    fclose (file);
    return 1;
}

int
check_errors(struct matrix *matrix, float scalar_value) {
    printf("Veficando erros...\n");
    float expected = scalar_value * matrix->rows[0];
    for(int i = 0; i < matrix->height * matrix->width; i++){
        if(matrix->rows[i] - expected > 0.0001){
            printf("Erro na matrix");
            return 0;
        } 
    }
    printf("Tudo certo!\n");
    return 1;
}



void
print_matrix (struct matrix *mat)
{
    int count = 0;
    for (int i = 0; i < mat->height; i++) 
    {
        for (int j = 0; j < mat->width; j++) 
        {
            printf("%.2f ", mat->rows[i * mat->width + j]);
            count++;
            if (count >= 256) 
            {
                printf("... (output truncated to 256 elements)\n");
                return;
            }
        }
    }
}



int
main (int argc, char *argv[])
{
    struct matrix matrixA, matrixB, matrixC;
    struct timeval start, stop, overall_t1, overall_t2;
    gettimeofday (&overall_t1, NULL);
    if (argc != 10)
    {
        printf
        ("Uso: %s <valor escalar> <altura matriz A> <largura matriz A> <altura matriz B> <largura matriz B> <arquivos>\n",
         argv[0]);

        return 1;
    }

    float scalar = atof (argv[1]);
    unsigned long heightA = strtoul (argv[2], NULL, 10);
    unsigned long widthA = strtoul (argv[3], NULL, 10);
    unsigned long heightB = strtoul (argv[4], NULL, 10);
    unsigned long widthB = strtoul (argv[5], NULL, 10);

    const char *fileA = argv[6];	// Primeiro arquivo binário para matriz A
    const char *fileB = argv[7];	// Segundo arquivo binário para matriz B
    const char *fileC = argv[8];	// Terceiro arquivo binário para matriz resultado de multiplicação escalar
    const char *fileD = argv[9];	// Quarto arquivo binário para matriz resultado da multiplicação matriz-matriz


    float elapsed_scalar_mult, elapsed_matrix_mult;


    // Alocando as matrizes
    matrixA.height = heightA;
    matrixA.width = widthA;
    matrixA.rows =
        (float*)aligned_alloc(32, matrixA.width * matrixA.height * sizeof(float));
    matrixB.height = heightB;
    matrixB.width = widthB;
    matrixB.rows =
        (float*)aligned_alloc(32, matrixB.width * matrixB.height * sizeof(float));
    matrixC.height = matrixA.height;
    matrixC.width = matrixB.width;
    matrixC.rows =
        (float*)aligned_alloc(32, matrixC.width * matrixC.height * sizeof(float));

    if (matrixA.rows == NULL || matrixB.rows == NULL
            || matrixC.rows == NULL)

    {
        printf ("Erro ao alocar memória para as matrizes\n");
        return 1;
    }

    // Lendo matrizes dos arquivos binários
    if (!read_matrix_from_file (fileA, &matrixA)
            ||!read_matrix_from_file (fileB, &matrixB))

    {
        printf ("Erro ao ler matrizes dos arquivos binários\n");
        free (matrixA.rows);
        free (matrixB.rows);
        free (matrixC.rows);
        return 1;
    }

    printf("Imprimindo matrix A:\n");
    print_matrix(&matrixA);
    printf("-------------------------------\n");
    printf("Imprimindo matrix B:\n");
    print_matrix(&matrixB);
    printf("-------------------------------\n");
    printf("Imprimindo matrix C:\n");
    print_matrix(&matrixC);
    printf("-------------------------------\n");
    printf("\n");

    // Multiplicação escalar
    printf ("Executing Scalar por %2.f.\n", scalar);
    printf("\n");
    gettimeofday (&start, NULL);
    if (!scalar_matrix_mult (scalar, &matrixA))
    {
        printf ("%s: scalar_matrix_mult problem.", argv[0]);
        return 1;
    }
    gettimeofday (&stop, NULL);

    elapsed_scalar_mult = timedifference_msec (start, stop);

    printf ("Tempo de execucao: %f ms\n", elapsed_scalar_mult);

    printf("\n");

    printf("Imprimindo matrix A:\n");
    print_matrix(&matrixA);

    // Escrevendo o resultado da multiplicação escalar em um arquivo binário
    if (!write_matrix_to_file (fileC, &matrixA))
    {
        printf ("Erro ao escrever matriz no arquivo binário\n");
        free (matrixA.rows);
        free (matrixB.rows);
        free (matrixC.rows);
        return 1;
    }

    printf ("Multiplicação de matrizes\n");

    gettimeofday (&start, NULL);

    if (!matrix_matrix_mult (&matrixA, &matrixB, &matrixC))
    {
        printf ("%s: matrix_matrix_mult problem.", argv[0]);
        return 1;
    }

    gettimeofday (&stop, NULL);

    elapsed_matrix_mult = timedifference_msec (start, stop);
    printf ("Tempo de Execucao: %f ms\n", elapsed_matrix_mult);

    // Escrevendo o resultado da multiplicação matriz-matriz em um arquivo binário
    if (!write_matrix_to_file (fileD, &matrixC))
    {
        printf ("Erro ao escrever matriz no arquivo binário\n");
        free (matrixA.rows);
        free (matrixB.rows);
        free (matrixC.rows);
        return 1;
    }


    printf ("Operações concluídas com sucesso!\n");
    printf("\n");

    printf("Imprimindo matrix A:\n");
    print_matrix(&matrixA);

    printf("-------------------------------\n");
    printf("Imprimindo matrix B:\n");
    print_matrix(&matrixB);
    printf("-------------------------------\n");

    printf("Imprimindo matrix C:\n");
    print_matrix(&matrixC);
    printf("-------------------------------\n");


    check_errors(&matrixA, scalar*matrixA.rows[0]);

    printf("-------------------------------\n");
    printf("Tempo execução escalar: %f ms\n", elapsed_scalar_mult);
    printf("Tempo execução matriz: %f ms\n", elapsed_matrix_mult);

    // Liberando memória
    free(matrixA.rows);
    free(matrixB.rows);
    free(matrixC.rows);

    gettimeofday (&overall_t2, NULL);
    
    float overall_time = timedifference_msec (overall_t1, overall_t2);
    printf ("Tempo total de execução: %f ms\n", overall_time);
    return 0;

}
