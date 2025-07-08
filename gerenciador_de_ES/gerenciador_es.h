#ifndef GERENCIADOR_ES_H
#define GERENCIADOR_ES_H

#include "../include/processo.h"
#include "../include/recursoES.h"

typedef struct {
    Recurso impressoras[2];
    Recurso scanner;
    Recurso modem;
    Recurso discos[2];
} RecursosDoSistema;

void inicializar_recursos(RecursosDoSistema* recursos);

int alocar_recursos(Processo* p, RecursosDoSistema* recursos);

void liberar_recursos(Processo* p, RecursosDoSistema* recursos);

#endif 