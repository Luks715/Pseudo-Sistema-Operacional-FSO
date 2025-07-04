#ifndef FILA_H 
#define FILA_H

#define max_processos 1000
#include "processo.h"

typedef struct {
    int inicio, fim;
    Processo processos[max_processos];
} Fila;

// Funções
int start_queue(Fila* fila);
int append(Fila* fila, Processo p);
int pop(Fila* fila, Processo* p);
int queue_len(Fila* fila);
int queue_empty(Fila* fila);
int queue_full(Fila* fila);

#endif 
