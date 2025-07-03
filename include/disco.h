#ifndef DISCO_H
#define DISCO_H

#define TAM_DISCO 1000
#include "diretorio.h"

typedef struct {
    char blocos[TAM_DISCO];  // 0 = livre, Nome do arquivo = ocupado
    int total_blocos;        // Quantidade total de blocos (definido no arquivo files.txt)
    Diretorio diretorio;     
} Disco;

#endif
