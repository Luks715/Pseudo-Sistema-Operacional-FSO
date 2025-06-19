#include "fila.h"
#include <stddef.h>

void inicializar_fila(Fila* fila) {
    fila->inicio = 0;
    fila->fim = 0;
}

int fila_vazia(Fila* fila) {
    return fila->inicio == fila->fim;
}

int fila_cheia(Fila* fila) {
    return (fila->fim + 1) % 1000 == fila->inicio;
}

int enfileirar(Fila* fila, Processo* p) {
    if (fila_cheia(fila)) return 0;
    fila->processos[fila->fim] = p;
    fila->fim = (fila->fim + 1) % 1000;
    return 1;
}

Processo* desenfileirar(Fila* fila) {
    if (fila_vazia(fila)) return NULL;
    Processo* p = fila->processos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % 1000;
    return p;
}
