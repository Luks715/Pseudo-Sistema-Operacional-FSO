#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "memoria.h"
#include "disco.h"
#include "fila.h"
#include "kernel.h"
#include "recursoES.h" 

int dispatcher(Memoria* memoria, Disco* disco, Fila* filas[], Kernel* kernel,
               Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]);

#endif
