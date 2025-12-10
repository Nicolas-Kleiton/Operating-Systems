/* * Arquivo: single_thread.c
 * Compilar: gcc single_thread.c -o single -lm
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define TAMANHO_LISTA 10000

// Função auxiliar para ordenar (necessária para mediana)
int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    printf("--- MODO SINGLE-THREAD ---\n");
    printf("Gerando %d numeros...\n", TAMANHO_LISTA);

    // Alocação
    int *lista = malloc(TAMANHO_LISTA * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < TAMANHO_LISTA; i++) {
        lista[i] = rand() % 101;
    }

    // INICIO DA MEDIÇÃO DE TEMPO
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // 1. Calcular Média
    double soma = 0;
    for (int i = 0; i < TAMANHO_LISTA; i++) soma += lista[i];
    double media = soma / TAMANHO_LISTA;

    // 2. Calcular Desvio Padrão
    double soma_sq = 0;
    for (int i = 0; i < TAMANHO_LISTA; i++) soma_sq += pow(lista[i], 2);
    double variancia = (soma_sq / TAMANHO_LISTA) - (media * media);
    double desvio = sqrt(variancia);

    // 3. Calcular Mediana (Requer cópia para não estragar a lista original se fosse usada depois)
    // No single thread, como é o último passo, poderíamos ordenar direto, 
    // mas vamos copiar para ser justo com o custo de memória do multi-thread.
    int *copia = malloc(TAMANHO_LISTA * sizeof(int));
    for(int i=0; i<TAMANHO_LISTA; i++) copia[i] = lista[i];
    
    qsort(copia, TAMANHO_LISTA, sizeof(int), comparar);
    
    double mediana;
    if (TAMANHO_LISTA % 2 == 0)
        mediana = (copia[TAMANHO_LISTA/2 - 1] + copia[TAMANHO_LISTA/2]) / 2.0;
    else
        mediana = copia[TAMANHO_LISTA/2];
    
    free(copia);

    // FIM DA MEDIÇÃO
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Media: %.2f\n", media);
    printf("Mediana: %.2f\n", mediana);
    printf("Desvio Padrao: %.2f\n", desvio);
    printf(">>> Tempo Total: %.6f segundos\n", tempo_gasto);

    free(lista);
    return 0;
}