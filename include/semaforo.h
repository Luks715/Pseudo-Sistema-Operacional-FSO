#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "fila.h"
#include "processo.h"

typedef struct {
    int valor;
    Fila processos_bloqueados; // Fila para processos esperando pelo semáforo
} Semaforo;

void iniciar_semaforo(Semaforo* s, int valor_inicial);

int wait(Semaforo* s, Processo* p_atual);

void signal(Semaforo* s);

#endif