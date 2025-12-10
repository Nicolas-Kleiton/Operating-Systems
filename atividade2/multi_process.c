/*
 * Atividade de Sistemas Operacionais
 * Objetivo: Fazer um programa que calcule a média, mediana e desvio padrão de uma lista de 10.000 números inteiros,
 * usando múltiplos processos (fork + pipes) para paralelizar os 3 cálculos.
 *
 * Alunos:
 * Gerson Gonçalves de Freitas, 20240007943
 * Nicolas Kleiton da Silva Melo, 20240009044
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

#define TAMANHO 10000

int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    printf("--- MULTI PROCESS (Fork + Pipes) ---\n");

    // 1. Gerar Dados (O pai gera, os filhos herdam a cópia)
    // Usamos static para garantir que não estoure a stack em alguns sistemas, 
    // embora 10k seja pequeno.
    static int dados[TAMANHO]; 
    srand(time(NULL));
    for(int i=0; i<TAMANHO; i++) dados[i] = rand() % 101;

    // Pipes para comunicação: [0] = leitura, [1] = escrita
    int pipe_media[2], pipe_desvio[2], pipe_mediana[2];
    
    if (pipe(pipe_media) < 0 || pipe(pipe_desvio) < 0 || pipe(pipe_mediana) < 0) {
        perror("Erro ao criar pipes");
        exit(1);
    }

    // INICIO CRONOMETRO
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // --- PROCESSO 1: CALCULA MÉDIA ---
    if (fork() == 0) {
        close(pipe_media[0]); // Fecha leitura, filho só escreve
        
        double soma = 0;
        for(int i=0; i<TAMANHO; i++) soma += dados[i];
        double res = soma / TAMANHO;
        
        write(pipe_media[1], &res, sizeof(double));
        close(pipe_media[1]);
        exit(0); // Filho termina
    }

    // --- PROCESSO 2: CALCULA DESVIO PADRÃO ---
    if (fork() == 0) {
        close(pipe_desvio[0]);
        
        double soma = 0, soma_sq = 0;
        for(int i=0; i<TAMANHO; i++) {
            soma += dados[i];
            soma_sq += dados[i] * dados[i];
        }
        double media = soma / TAMANHO;
        double variancia = (soma_sq / TAMANHO) - (media * media);
        double res = sqrt(variancia);
        
        write(pipe_desvio[1], &res, sizeof(double));
        close(pipe_desvio[1]);
        exit(0);
    }

    // --- PROCESSO 3: CALCULA MEDIANA ---
    if (fork() == 0) {
        close(pipe_mediana[0]);
        
        // Ordena o array (que é uma cópia herdada do pai)
        qsort(dados, TAMANHO, sizeof(int), comparar);
        
        double res;
        if(TAMANHO % 2 == 0)
            res = (dados[TAMANHO/2 - 1] + dados[TAMANHO/2]) / 2.0;
        else
            res = dados[TAMANHO/2];
            
        write(pipe_mediana[1], &res, sizeof(double));
        close(pipe_mediana[1]);
        exit(0);
    }

    // --- CÓDIGO DO PAI ---
    
    // Fecha as pontas de escrita dos pipes (pai só lê)
    close(pipe_media[1]);
    close(pipe_desvio[1]);
    close(pipe_mediana[1]);

    double res_media, res_desvio, res_mediana;

    // Lê os resultados vindos dos filhos
    read(pipe_media[0], &res_media, sizeof(double));
    read(pipe_desvio[0], &res_desvio, sizeof(double));
    read(pipe_mediana[0], &res_mediana, sizeof(double));

    // Espera todos os filhos morrerem para evitar processos "zumbis"
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // FIM CRONOMETRO
    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Media: %.2f | Desvio: %.2f | Mediana: %.2f\n", res_media, res_desvio, res_mediana);
    printf("Tempo de Execucao: %.6f segundos\n", tempo);

    return 0;
}