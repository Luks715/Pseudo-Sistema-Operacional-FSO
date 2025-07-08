#ifndef KERNEL_H
#define KERNEL_H

#define TAM_OP 1000

#include "operacao.h"
#include "disco.h"
#include "processo.h"
#include "memoria.h"

typedef struct{
    Operacao operacoes[TAM_OP];  // Armazena todas as operações que os processos devem realizar
    int num_operacoes;           // Quantidade de operações que o sistema deve realizar

    // Ponteiro para a função que realiza as funções create e delete de arquivos
    int (*sistema_arquivos)(int, const char*, int, Disco*, Processo*);

    // Ponteiro para a função que aloca memória para processos na RAM
    int (*alocador)(Processo*, Memoria*);
} Kernel;

#endif