#ifndef DIRETORIO_H
#define DIRETORIO_H

#include "arquivo.h"

#define MAX_ARQUIVOS 100

typedef struct {
    Arquivo arquivos[MAX_ARQUIVOS]; // Lista de arquivos
    int total_arquivos;             // Quantos arquivos foram criados
} Diretorio;

#endif
