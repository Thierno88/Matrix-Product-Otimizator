#include <stdio.h>
#include <stdlib.h>

int main() {
    // Definindo o tamanho da matriz (8x8 neste exemplo)
    int linhas = 2048;
    int colunas = 2048;

    // Alocando dinamicamente uma matriz de tamanho múltiplo de 8
    float **matriz = (float **)malloc(linhas * sizeof(float *));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (float *)malloc(colunas * sizeof(float));
    }

    // Loop para criar e gravar 4 matrizes
    for (int mat_num = 1; mat_num <= 4; mat_num++) {
        // Preenchendo a matriz com valores (exemplo: números de ponto flutuante)
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                matriz[i][j] = 2.0f;  // Modificando o valor conforme a matriz
            }
        }

        // Criando o nome do arquivo dinamicamente
        char nome_arquivo[20];
        sprintf(nome_arquivo, "mat%d.dat", mat_num);

        // Abrindo o arquivo binário para escrita
        FILE *arquivo = fopen(nome_arquivo, "wb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo %s.\n", nome_arquivo);
            return 1;
        }

        // Gravando a matriz no arquivo binário
        for (int i = 0; i < linhas; i++) {
            fwrite(matriz[i], sizeof(float), colunas, arquivo);
        }

        // Fechando o arquivo
        fclose(arquivo);

        printf("Matriz %d escrita no arquivo %s com sucesso.\n", mat_num, nome_arquivo);
    }

    // Liberando a memória alocada para a matriz
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
