/*
 * Atividade de Sistemas Operacionais
 * Objetivo: Fazer um programa que calcule a média, mediana e desvio padrão de uma lista de 10.000 números inteiros,
 * entre 0 e 100 usando um único processo (sequencial).
 *
 * Alunos:
 * Gerson Gonçalves de Freitas, 20240007943
 * Nicolas Kleiton da Silva Melo, 20240009044
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define TAMANHO 10000

// Função de comparação para o qsort
int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    printf("--- SINGLE PROCESS (Sequencial) ---\n");
    
    // 1. Gerar Dados
    int dados[TAMANHO];
    srand(time(NULL));
    for(int i=0; i<TAMANHO; i++) dados[i] = rand() % 101;

    // INICIO CRONOMETRO
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // 2. Calcular Média
    double soma = 0;
    for(int i=0; i<TAMANHO; i++) soma += dados[i];
    double media = soma / TAMANHO;

    // 3. Calcular Desvio Padrão
    double soma_sq = 0;
    for(int i=0; i<TAMANHO; i++) soma_sq += pow(dados[i], 2);
    double variancia = (soma_sq / TAMANHO) - (media * media);
    double desvio = sqrt(variancia);

    // 4. Calcular Mediana
    // Copiamos para não alterar o array original (embora aqui não faria diferença, é boa prática)
    int copia[TAMANHO];
    for(int i=0; i<TAMANHO; i++) copia[i] = dados[i];
    qsort(copia, TAMANHO, sizeof(int), comparar);
    
    double mediana;
    if(TAMANHO % 2 == 0)
        mediana = (copia[TAMANHO/2 - 1] + copia[TAMANHO/2]) / 2.0;
    else
        mediana = copia[TAMANHO/2];

    // FIM CRONOMETRO
    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Media: %.2f | Desvio: %.2f | Mediana: %.2f\n", media, desvio, mediana);
    printf("Tempo de Execucao: %.6f segundos\n", tempo);

    return 0;
}