#ifndef FILAS_H
#define FILAS_H

#define max_processos 1000
#include "processo.h"

typedef struct{
    int inicio, fim;
    Processo processos[max_processos];
} Fila;

// Funções
int append(Fila* fila, Processo p);
int pop(Fila* fila, Processo* p);

#endif
