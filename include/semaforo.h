#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "fila.h"

typedef struct {
    int valor;
    Fila processos_bloqueados; // Fila para processos esperando pelo semáforo
} Semaforo;

void iniciar_semaforo(Semaforo* s, int valor_inicial);

void wait(Semaforo* s, Processo* p_atual, Fila* fila_prontos_origem);

void signal(Semaforo* s);

#endif