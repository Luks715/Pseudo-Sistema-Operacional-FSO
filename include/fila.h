#ifndef FILAS_H
#define FILAS_H

#define max_processos 1000
#include "processo.h"

typedef struct{
    int inicio, fim;
    Processo processos[max_processos];
} Fila;

#endif