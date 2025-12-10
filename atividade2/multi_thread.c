/*
 * Atividade de Sistemas Operacionais
 * Objetivo: Fazer um programa que calcule a média, mediana e desvio padrão de uma lista de 10.000 números inteiros,
 * entre 0 e 100 usando um modelo multi-thread de 3 threads (pthread.h) para paralelizar os 3 cálculos.
 *
 * Alunos:
 * Gerson Gonçalves de Freitas, 20240007943
 * Nicolas Kleiton da Silva Melo, 20240009044
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define TAMANHO_LISTA 10000

// Variável global para as threads acessarem (apenas leitura)
int *lista_global;

// Função auxiliar para qsort
int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// --- THREAD 1: MEDIA ---
void* calcular_media(void* arg) {
    double soma = 0;
    for (int i = 0; i < TAMANHO_LISTA; i++) {
        soma += lista_global[i];
    }
    
    double *res = malloc(sizeof(double));
    *res = soma / TAMANHO_LISTA;
    pthread_exit((void*)res);
}

// --- THREAD 2: DESVIO PADRAO ---
void* calcular_desvio(void* arg) {
    double soma = 0;
    double soma_sq = 0;
    
    // Calcula somas locais para evitar dependência da thread de média
    for (int i = 0; i < TAMANHO_LISTA; i++) {
        double val = lista_global[i];
        soma += val;
        soma_sq += val * val;
    }
    
    double media = soma / TAMANHO_LISTA;
    double variancia = (soma_sq / TAMANHO_LISTA) - (media * media);
    
    double *res = malloc(sizeof(double));
    *res = sqrt(variancia);
    pthread_exit((void*)res);
}

// --- THREAD 3: MEDIANA ---
void* calcular_mediana(void* arg) {
    // Aloca cópia local para ordenar sem travar as outras threads
    int *copia = malloc(TAMANHO_LISTA * sizeof(int));
    for(int i=0; i<TAMANHO_LISTA; i++) copia[i] = lista_global[i];
    
    qsort(copia, TAMANHO_LISTA, sizeof(int), comparar);
    
    double *res = malloc(sizeof(double));
    if (TAMANHO_LISTA % 2 == 0)
        *res = (copia[TAMANHO_LISTA/2 - 1] + copia[TAMANHO_LISTA/2]) / 2.0;
    else
        *res = (double)copia[TAMANHO_LISTA/2];
    
    free(copia);
    pthread_exit((void*)res);
}

int main() {
    printf("--- MODO MULTI-THREAD (3 Threads) ---\n");
    printf("Gerando %d numeros...\n", TAMANHO_LISTA);

    lista_global = malloc(TAMANHO_LISTA * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < TAMANHO_LISTA; i++) {
        lista_global[i] = rand() % 101;
    }

    // INICIO DA MEDIÇÃO DE TEMPO
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    pthread_t t1, t2, t3;
    
    // Dispara as 3 tarefas simultaneamente
    pthread_create(&t1, NULL, calcular_media, NULL);
    pthread_create(&t2, NULL, calcular_desvio, NULL);
    pthread_create(&t3, NULL, calcular_mediana, NULL);

    // Aguarda e recolhe resultados
    double *res_media, *res_desvio, *res_mediana;
    
    pthread_join(t1, (void**)&res_media);
    pthread_join(t2, (void**)&res_desvio);
    pthread_join(t3, (void**)&res_mediana);

    // FIM DA MEDIÇÃO
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Media: %.2f\n", *res_media);
    printf("Mediana: %.2f\n", *res_mediana);
    printf("Desvio Padrao: %.2f\n", *res_desvio);
    printf(">>> Tempo Total: %.6f segundos\n", tempo_gasto);

    free(lista_global);
    free(res_media);
    free(res_desvio);
    free(res_mediana);
    
    return 0;
}