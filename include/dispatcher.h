#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "memoria.h"
#include "disco.h"
#include "fila.h"
#include "recursoES.h" 

int dispatcher(Memoria* memoria, Disco* disco, Fila* fila_global,
               Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]);

#endif
