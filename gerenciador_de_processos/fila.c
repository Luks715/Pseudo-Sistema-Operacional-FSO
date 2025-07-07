#include "fila.h"

int start_queue(Fila* fila) {
    fila->inicio = 0;
    fila->fim = 0;
    return 0; 
}

int append(Fila* fila, Processo p) {
    int proximo = (fila->fim + 1) % max_processos;

    if (proximo == fila->inicio) {
        return -1;  // fila cheia
    }

    fila->processos[fila->fim] = p;
    fila->fim = proximo;
    return 0;
}

int pop(Fila* fila, Processo* p) {
    if (fila->inicio == fila->fim) {
        return -1;  // fila vazia
    }

    *p = fila->processos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % max_processos;
    return 0;
}

int queue_len(Fila* fila) {
    return (fila->fim - fila->inicio + max_processos) % max_processos;
}

int queue_empty(Fila* fila) {
    return fila->inicio == fila->fim;
}

int queue_full(Fila* fila) {
    return ((fila->fim + 1) % max_processos) == fila->inicio;
}