#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "processo.h"
#include "fila.h"


void escalonar();

void executar_processo(Processo* p);

void aging(Fila* fila, Fila* fila_superior);

void rebaixar_processo(Processo* p);


#endif 