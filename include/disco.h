#ifndef DISCO_H
#define DISCO_H

#define TAM_DISCO 1000
#include "diretorio.h"
#include "operacao.h"

typedef struct {
    char blocos[TAM_DISCO];  // 0 = livre, Nome do arquivo = ocupado
    int total_blocos;        // Quantidade total de blocos (definido no arquivo files.txt)
    Diretorio diretorio;     
    Operacao operacoes[TAM_DISCO];  // Armazena todas as operações que os processos devem realizar

    // Ponteiro para a função que realiza as funções create e delete
    int (*sistema_arquivos)(int, const char*, int, struct Disco*, struct Processo*);
} Disco;

#endif
