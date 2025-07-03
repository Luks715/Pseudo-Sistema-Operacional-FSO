#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "memoria.h"
#include "disco.h"
#include "fila.h"
#include "kernel.h"

void dispatcher(Memoria* memoria, Disco* disco, Fila* fila_global, Kernel* kernel);

#endif
