/*
Seu programa deve:

Caso na linha de comando for passado o argumento -n seguido de um numero: 
Ex. mult -n 256: Entao o programa gera 2 matrizes quadradas aleatorias, do tamanho indicado na linha de comando, neste caso, 256x256, 
e deve entao multiplica-las primeiramente com o metodo tradicional de multiplicacao de matrizes, 
e depois com o metodo de strassen. Seu programa deve medir o tempo de execucao de cada algoritmo, 
e imprimi-lo.

Voce deve comprarar a performance para tamanhos diferentes, e se quizer, 
pode tentar melhorar o strassen fazendo um algoritmo hibrido -- que use o metodo tradicional para matrizes pequenas.

Caso a linha de comando nao contenha nenhum argumento, neste caso o seu programa deve:

ler um numero do teclado, 
que indicara o tamanho das matrizes a serem lidas 
ler as matrizes do teclado multiplica-las com cada um dos metodos 
e imprimir a matriz obtida em cada metodo. 
(Veja o exemplo de entrada e saida enviando um programa do tipo hello world)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define EXIBIR_TEMPO_GASTO_EM_FUNC true
#define REALIZAR_ALGORITMO_HIBRIDO true
#define EXIBIR_RESULTADO_MATRIZES true
#define TAM_MATRIZ_PEQUENA 400ul

void* alocar_matriz_quadrada(size_t tam);
void desalocar_matriz_quadrada(size_t tam, int (*matriz)[tam]);
void preencher_matriz_quadrada_aleatoriamente(size_t tam, int matriz[tam][tam]);
void preencher_matriz_quadrada_input(size_t tam, int matriz[tam][tam]);

void* multiplicao_tradicional(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas]);

void* soma_matriz(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas]);

void* subtracao_matriz(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas]);

void* multiplicao_strassen(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas]);
void* split_strassen(size_t tam, int matriz_split[tam][tam], int quarto_do_vetor_0_based);
void* join_strassen(size_t tam, int matriz11[tam/2][tam/2], int matriz12[tam/2][tam/2], int matriz21[tam/2][tam/2], int matriz22[tam/2][tam/2]);

void print_matriz(size_t qtd_linhas, size_t qtd_colunas, int matriz[qtd_linhas][qtd_colunas]);

void* realizar_multiplicacao_tradicional(size_t tam, int matrizA[tam][tam], int matrizB[tam][tam], 
    bool exibir_tempo, bool exibir_matriz);
void* realizar_multiplicacao_strassen(size_t tam, int matrizA[tam][tam], int matrizB[tam][tam], 
    bool exibir_tempo, bool exibir_matriz, bool realizar_alg_hibrido);

int main(int argc, char const *argv[])
{
    long tam_matriz = 0;
    bool input_pelo_usuario = true;

    if(argc == 3)
    {
        char const* parametro_n = argv[1];
        char const* tam_matriz_str = argv[2];
        
        if(strncmp(parametro_n, "-n", 3) == 0) {
            tam_matriz = strtol(tam_matriz_str, NULL, 10);
            input_pelo_usuario = false;
        }
        else
        {
            puts("Parâmetros enviados inválidos");
            return -1;
        }
    } else {
        scanf("%ld", &tam_matriz);
        input_pelo_usuario = true;
    }

    //Verificar se o tam_matriz não é uma potência de dois
    if(!(!(tam_matriz & (tam_matriz - 1)) && tam_matriz))
    {
        puts("Tamanho da matriz inválido. O tamanho da matriz precisa ser uma potência de 2.");
        return -1;
    }

    int (*matriz_a)[tam_matriz] = alocar_matriz_quadrada(tam_matriz);
    int (*matriz_b)[tam_matriz] = alocar_matriz_quadrada(tam_matriz);

    if(input_pelo_usuario) 
    {
        preencher_matriz_quadrada_input(tam_matriz, matriz_a);
        preencher_matriz_quadrada_input(tam_matriz, matriz_b);
    }
    else
    {
        srand(time(NULL));
        preencher_matriz_quadrada_aleatoriamente(tam_matriz, matriz_a);
        preencher_matriz_quadrada_aleatoriamente(tam_matriz, matriz_b);
    }

    int (*matriz_resultado_trad)[tam_matriz] = realizar_multiplicacao_tradicional(tam_matriz, matriz_a, matriz_b, 
        EXIBIR_TEMPO_GASTO_EM_FUNC, EXIBIR_RESULTADO_MATRIZES);

    int (*matriz_resultado_strassen)[tam_matriz] = realizar_multiplicacao_strassen(tam_matriz, matriz_a, matriz_b, 
        EXIBIR_TEMPO_GASTO_EM_FUNC, EXIBIR_RESULTADO_MATRIZES, REALIZAR_ALGORITMO_HIBRIDO);

    desalocar_matriz_quadrada(tam_matriz, matriz_a);
    desalocar_matriz_quadrada(tam_matriz, matriz_b);
    desalocar_matriz_quadrada(tam_matriz, matriz_resultado_trad);
    desalocar_matriz_quadrada(tam_matriz, matriz_resultado_strassen);

    return 0;
}

void* alocar_matriz_quadrada(size_t tam)
{
    int (*matriz)[tam] = calloc(sizeof(*matriz), tam);

    return matriz;
}

void desalocar_matriz_quadrada(size_t tam, int (*matriz)[tam])
{
    free(matriz);
}

void preencher_matriz_quadrada_aleatoriamente(size_t tam, int matriz[tam][tam])
{
    const int MAX_RAND_INT = 5;

    for(size_t linha = 0; linha < tam; linha++)
    {
        for(size_t coluna = 0; coluna < tam; coluna++)
        {
            int num_aleatorio = rand() % (MAX_RAND_INT + 1);
            matriz[linha][coluna] = num_aleatorio;
        }
    }
}

void preencher_matriz_quadrada_input(size_t tam, int matriz[tam][tam])
{
    for(size_t linha = 0; linha < tam; linha++)
    {
        for(size_t coluna = 0; coluna < tam; coluna++)
        {
            scanf("%d", &matriz[linha][coluna]);
        }
    }
}

void* multiplicao_tradicional(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas])
{
    size_t tam = qtd_linhas;
    int (*matriz_out)[tam] = alocar_matriz_quadrada(tam);
    for(size_t linha = 0; linha < qtd_linhas; linha++)
    {
        for(size_t coluna = 0; coluna < qtd_colunas; coluna++)
        {
            for(size_t k = 0; k < tam; k++)
            {
                matriz_out[linha][coluna] += matriz_a[linha][k] * matriz_b[k][coluna];
            }
        }
    }

    return matriz_out;
}

void* soma_matriz(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas])
{
    size_t tam = qtd_linhas;
    int (*matriz_out)[tam] = alocar_matriz_quadrada(tam);

    for(size_t linha = 0; linha < qtd_linhas; linha++)
    {
        for(size_t coluna = 0; coluna < qtd_colunas; coluna++)
        {
            matriz_out[linha][coluna] = matriz_a[linha][coluna] + matriz_b[linha][coluna];
        }
    }

    return matriz_out;
}

void* subtracao_matriz(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas])
{
    size_t tam = qtd_linhas;
    int (*matriz_out)[tam] = alocar_matriz_quadrada(tam);

    for(size_t linha = 0; linha < qtd_linhas; linha++)
    {
        for(size_t coluna = 0; coluna < qtd_colunas; coluna++)
        {
            matriz_out[linha][coluna] = matriz_a[linha][coluna] - matriz_b[linha][coluna];
        }
    }

    return matriz_out;
}

void* multiplicao_strassen(size_t qtd_linhas, size_t qtd_colunas, 
    int matriz_a[qtd_linhas][qtd_colunas], 
    int matriz_b[qtd_linhas][qtd_colunas])
{
    size_t tam = qtd_linhas;
    size_t metade_tam = tam/2;
    
    if(tam == 1)
    {
        int (*matriz_out)[1] = alocar_matriz_quadrada(1);
        matriz_out[0][0] = matriz_a[0][0] * matriz_b[0][0];
        return matriz_out;
    }

    int(*a)[metade_tam] = split_strassen(tam, matriz_a, 0);
    int(*b)[metade_tam] = split_strassen(tam, matriz_a, 1);
    int(*c)[metade_tam] = split_strassen(tam, matriz_a, 2);
    int(*d)[metade_tam] = split_strassen(tam, matriz_a, 3);

    int(*e)[metade_tam] = split_strassen(tam, matriz_b, 0);
    int(*f)[metade_tam] = split_strassen(tam, matriz_b, 1);
    int(*g)[metade_tam] = split_strassen(tam, matriz_b, 2);
    int(*h)[metade_tam] = split_strassen(tam, matriz_b, 3);
    
    int(*a_mais_d)[metade_tam] = soma_matriz(metade_tam, metade_tam, a, d);
    int(*e_mais_h)[metade_tam] = soma_matriz(metade_tam, metade_tam, e, h);
    int(*g_menos_e)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, g, e);
    int(*a_mais_b)[metade_tam] = soma_matriz(metade_tam, metade_tam, a, b);
    int(*b_menos_d)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, b, d);
    int(*g_mais_h)[metade_tam] = soma_matriz(metade_tam, metade_tam, g, h);
    int(*f_menos_h)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, f, h);
    int(*c_mais_d)[metade_tam] = soma_matriz(metade_tam, metade_tam, c, d);
    int(*a_menos_c)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, a, c);
    int(*e_mais_f)[metade_tam] = soma_matriz(metade_tam, metade_tam, e, f);

    int(*p1)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, a_mais_d, e_mais_h);
    int(*p2)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, d, g_menos_e);
    int(*p3)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, a_mais_b, h);
    int(*p4)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, b_menos_d, g_mais_h);
    int(*p5)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, a, f_menos_h);
    int(*p6)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, c_mais_d, e);
    int(*p7)[metade_tam] = multiplicao_strassen(metade_tam, metade_tam, a_menos_c, e_mais_f);

    desalocar_matriz_quadrada(metade_tam, a);
    desalocar_matriz_quadrada(metade_tam, b);
    desalocar_matriz_quadrada(metade_tam, c);
    desalocar_matriz_quadrada(metade_tam, d);
    desalocar_matriz_quadrada(metade_tam, e);
    desalocar_matriz_quadrada(metade_tam, f);
    desalocar_matriz_quadrada(metade_tam, g);
    desalocar_matriz_quadrada(metade_tam, h);
    desalocar_matriz_quadrada(metade_tam, a_mais_d);
    desalocar_matriz_quadrada(metade_tam, e_mais_h);
    desalocar_matriz_quadrada(metade_tam, g_menos_e);
    desalocar_matriz_quadrada(metade_tam, a_mais_b);
    desalocar_matriz_quadrada(metade_tam, b_menos_d);
    desalocar_matriz_quadrada(metade_tam, g_mais_h);
    desalocar_matriz_quadrada(metade_tam, f_menos_h);
    desalocar_matriz_quadrada(metade_tam, c_mais_d);
    desalocar_matriz_quadrada(metade_tam, a_menos_c);
    desalocar_matriz_quadrada(metade_tam, e_mais_f);

    // Criando C11:
    int(*p1_mais_p2)[metade_tam] = soma_matriz(metade_tam, metade_tam, p1, p2);

    int(*p1_mais_p2_menos_p3)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, p1_mais_p2, p3);
    desalocar_matriz_quadrada(metade_tam, p1_mais_p2);

    int(*p1_mais_p2_menos_p3_mais_p4)[metade_tam] = soma_matriz(metade_tam, metade_tam, p1_mais_p2_menos_p3, p4);
    desalocar_matriz_quadrada(metade_tam, p1_mais_p2_menos_p3);

    int(*C11)[metade_tam] = p1_mais_p2_menos_p3_mais_p4;
    //Finalizado criação do C11

    // Criando C12:
    int(*C12)[metade_tam] = soma_matriz(metade_tam, metade_tam, p5, p3);
    //Finalizado criação do C12

    // Criando C21:
    int(*C21)[metade_tam] = soma_matriz(metade_tam, metade_tam, p6, p2);
    //Finalizado criação do C21

    // Criando C22:
    int(*p5_mais_p1)[metade_tam] = soma_matriz(metade_tam, metade_tam, p5, p1);

    int(*p5_mais_p1_menos_p6)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, p5_mais_p1, p6);
    desalocar_matriz_quadrada(metade_tam, p5_mais_p1);

    int(*p5_mais_p1_menos_p6_menos_p7)[metade_tam] = subtracao_matriz(metade_tam, metade_tam, p5_mais_p1_menos_p6, p7);
    desalocar_matriz_quadrada(metade_tam, p5_mais_p1_menos_p6);

    int(*C22)[metade_tam] = p5_mais_p1_menos_p6_menos_p7;
    //Finalizado criação do C22

    desalocar_matriz_quadrada(metade_tam, p1);
    desalocar_matriz_quadrada(metade_tam, p2);
    desalocar_matriz_quadrada(metade_tam, p3);
    desalocar_matriz_quadrada(metade_tam, p4);
    desalocar_matriz_quadrada(metade_tam, p5);
    desalocar_matriz_quadrada(metade_tam, p6);
    desalocar_matriz_quadrada(metade_tam, p7);

    int(*matriz_out)[tam] = join_strassen(tam, C11, C12, C21, C22);

    desalocar_matriz_quadrada(metade_tam, C11);
    desalocar_matriz_quadrada(metade_tam, C12);
    desalocar_matriz_quadrada(metade_tam, C21);
    desalocar_matriz_quadrada(metade_tam, C22);

    return matriz_out;
}

void* split_strassen(size_t tam, int matriz_split[tam][tam], int quarto_do_vetor_0_based)
{
    size_t metade_tam = tam/2;

    int (*matriz_quarto)[metade_tam] = alocar_matriz_quadrada(metade_tam);

    for(size_t linha = 0; linha < metade_tam; linha++)
    {
        for(size_t coluna = 0; coluna < metade_tam; coluna++)
        {
            if(quarto_do_vetor_0_based == 0)
            {
                matriz_quarto[linha][coluna] = matriz_split[linha][coluna];
            }
            else if(quarto_do_vetor_0_based == 1)
            {
                matriz_quarto[linha][coluna] = matriz_split[linha][coluna + metade_tam];
            }
            else if(quarto_do_vetor_0_based == 2)
            {
                matriz_quarto[linha][coluna] = matriz_split[linha + metade_tam][coluna];
            }
            else if(quarto_do_vetor_0_based == 3)
            {
                matriz_quarto[linha][coluna] = matriz_split[linha + metade_tam][coluna + metade_tam];
            }
        }
    }

    return matriz_quarto;
}

void* join_strassen(size_t tam, int matriz11[tam/2][tam/2], int matriz12[tam/2][tam/2], int matriz21[tam/2][tam/2], int matriz22[tam/2][tam/2]) 
{
    size_t metade_tam = tam/2;
    int (*matriz_join)[tam] = alocar_matriz_quadrada(tam);
    for (size_t linha = 0; linha < metade_tam; linha++)
    {
        for (size_t coluna = 0; coluna < metade_tam; coluna++) 
        {
            matriz_join[linha][coluna] = matriz11[linha][coluna];
            matriz_join[linha][coluna + metade_tam] = matriz12[linha][coluna];
            matriz_join[metade_tam + linha][coluna] = matriz21[linha][coluna];
            matriz_join[linha + metade_tam][coluna + metade_tam] = matriz22[linha][coluna];
        }
    }

    return matriz_join;
}

void print_matriz(size_t qtd_linhas, size_t qtd_colunas, int matriz[qtd_linhas][qtd_colunas])
{
    for(size_t linha = 0; linha < qtd_linhas; linha++)
    {
        for(size_t coluna = 0; coluna < qtd_colunas; coluna++)
        {
            printf("%d", matriz[linha][coluna]);
            
            if(coluna != (qtd_colunas - 1))
            {
                printf("%s", " ");
            }
        }
        puts("");
    }
}

void* realizar_multiplicacao_tradicional(size_t tam, int matrizA[tam][tam], int matrizB[tam][tam],
    bool exibir_tempo, bool exibir_matriz)
{
    const char STR_ALG_TRADICIONAL[] = "Tradicional";

    clock_t inicio = clock();
 
    int (*matriz_resultado)[tam] = multiplicao_tradicional(tam, tam, matrizA, matrizB);
 
    clock_t fim = clock();

    double tempo_em_segundos_decimais = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    if(exibir_tempo) {
        printf("Tempo gasto com execução do algoritmo %s(segundos decimais): %lf\n", STR_ALG_TRADICIONAL, tempo_em_segundos_decimais);
    }

    if(exibir_matriz)
    {
        printf("%s:\n", STR_ALG_TRADICIONAL);
        print_matriz(tam, tam, matriz_resultado);
    }

    return matriz_resultado;
}

void* realizar_multiplicacao_strassen(size_t tam, int matrizA[tam][tam], int matrizB[tam][tam],
    bool exibir_tempo, bool exibir_matriz, bool realizar_alg_hibrido)
{
    const char STR_ALG_HIBRIDO[] = "Hibrido";
    const char STR_ALG_STRASSEN[] = "Strassen";

    clock_t inicio = clock();
    
    int (*matriz_resultado)[tam];
    if(realizar_alg_hibrido && tam < TAM_MATRIZ_PEQUENA) {
        matriz_resultado = multiplicao_tradicional(tam, tam, matrizA, matrizB);
    } else {
        matriz_resultado = multiplicao_strassen(tam, tam, matrizA, matrizB);
    }
 
    clock_t fim = clock();

    double tempo_em_segundos_decimais = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    if(exibir_tempo) {
        printf("Tempo gasto com execução do algoritmo %s(segundos decimais): %lf\n", 
            realizar_alg_hibrido ? STR_ALG_HIBRIDO : STR_ALG_STRASSEN, 
            tempo_em_segundos_decimais);
    }

    if(exibir_matriz) {
        printf("%s:\n", realizar_alg_hibrido ? STR_ALG_HIBRIDO : STR_ALG_STRASSEN);
        print_matriz(tam, tam, matriz_resultado);
    }

    return matriz_resultado;
}

