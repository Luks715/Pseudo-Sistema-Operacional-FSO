#ifndef RECURSOSES_H
#define RECURSOSES_H

#include "processo.h" 

typedef struct {
    int ocupado;
    int pid;
} Recurso;

/* --- INTERFACE DO MÓDULO DE RECURSO --- */

// Inicializa os recursos de E/S
void inicializar_recursos(Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]);

// Tenta alocar os recursos para um processo, garantindo uso exclusivo [cite: 14, 57]
int alocar_recursos(Processo* p, Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]);

// Libera os recursos que um processo estava utilizando
void liberar_recursos(Processo* p, Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]);

#endif // RECURSOSES_H