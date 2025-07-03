#ifndef MEMORIA_H
#define MEMORIA_H

#include "bloco.h"

#define TOTAL_BLOCOS 1024
#define BLOCOS_TEMPO_REAL 64
#define BLOCOS_USUARIO 960

typedef struct {
    Bloco blocos[TOTAL_BLOCOS];
} Memoria;

#endif
