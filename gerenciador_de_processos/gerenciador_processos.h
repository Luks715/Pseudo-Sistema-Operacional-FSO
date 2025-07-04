#ifndef GERENCIADOR_PROCESSOS_H
#define GERENCIADOR_PROCESSOS_H

#include "fila.h"
#include "processo.h"

void despachar_processo(Processo* processo, Fila filas[]);

#endif
