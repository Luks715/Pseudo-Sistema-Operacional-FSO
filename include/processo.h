#ifndef PROCESSO_H
#define PROCESSO_H

#include <stdbool.h>

typedef struct {
    int pid;

    int tempo_inicializacao;
    int prioridade;
    int tempo_de_processador;
    int blocos_mem;
    int impressora;
    int scanner;
    int modem;
    int codigo_disco;

    int offset_mem;
    int ciclos_na_fila; // usado para aging
    int tempo_executado; // usado pelo escalonador
    bool executando;
} Processo;

#endif