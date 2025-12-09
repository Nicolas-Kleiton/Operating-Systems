/*
 * Atividade de Sistemas Operacionais
 * Objetivo: Fazer um programa que divida em 7 processos formando uma árvore de acordo com a figura. 
 * Cada processo de N1 a N4 deve executar um comando diferente do terminal do Linux a sua escolha 
 * usando o comando execl do unistd.h). 
 *
 * Quando todos os processos N terminarem, os processos F1 e F2 deverão imprimir uma mensagem 
 * indicando que eles são processos filhos, e imprimir seus PID e o do seu pai P1. 
 * Apenas quando eles terminarem, o processo P1 deve imprimir a mensagem que ele é o pai e seu PID, 
 * seguida de uma mensagem de encerramento do programa
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

/**
 * Função: criarNeto
 * -----------------
 * Executa execl().
 */
void criarNeto(const char* comando) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("[ERRO] Falha no Fork (Neto)");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        // --- Processo Neto ---
        // Executa o comando e substitui a imagem do processo
        execl("/bin/sh", "sh", "-c", comando, (char*)NULL);

        // Só chega aqui se o execl falhar
        perror("[ERRO] Falha na execucao (execl)");
        exit(EXIT_FAILURE);
    }
}

/**
 * Função: criarFilho
 * ------------------
 * Gerencia dois netos e reporta término técnico.
 */
void criarFilho(const char* cmdNetoA, const char* cmdNetoB) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("[ERRO] Falha no Fork (Filho)");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        // --- Processo Filho ---

        criarNeto(cmdNetoA);
        criarNeto(cmdNetoB);

        // Aguarda término dos processos netos (sincronização)
        wait(NULL);
        wait(NULL);

        // Saída técnica em Português
        printf("[ENCERRAMENTO_FILHO] PID: %d | PPID: %d | STATUS: NETOS_FINALIZADOS\n", 
               getpid(), getppid());

        exit(EXIT_SUCCESS);
    }
}

/**
 * Função: main
 * ------------
 * Processo Pai (Raiz).
 */
int main() {
    // Cabeçalho técnico indicando início da execução
    printf("[INICIO_SISTEMA] PID Raiz: %d\n\n", getpid());

    // Criação dos sub-processos
    criarFilho("ls -l", "pwd");
    criarFilho("date", "whoami");

    // Sincronização: Chamada bloqueante (wait)
    wait(NULL);
    wait(NULL);

    // Mensagem final técnica solicitada
    printf("\n[SAIDA_PRINCIPAL] PID: %d | EVENTO: SINCRONIZACAO_CONCLUIDA | ACAO: FINALIZANDO_PROGRAMA\n", getpid());

    return 0;
}