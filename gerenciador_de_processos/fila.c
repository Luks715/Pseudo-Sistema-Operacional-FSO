#include "fila.h"

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
