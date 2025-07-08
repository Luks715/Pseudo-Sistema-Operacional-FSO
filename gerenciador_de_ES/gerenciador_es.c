#include <stdio.h>
#include "gerenciador_es.h"
#include "../include/semaforo.h"

// Declaração para que este módulo "enxergue" os semáforos globais definidos no main.c
extern Semaforo sem_impressora;
extern Semaforo sem_scanner;
extern Semaforo sem_modem;
extern Semaforo sem_disco_sata;

void alocar_recursos(Processo* p) {
    // Processos de tempo real não alocam recursos de E/S
    if (p->prioridade == 0) {
        return;
    }

    printf("dispatcher => Processo %d requisitando recursos de E/S...\n", p->pid);

    if (p->impressora >= 0) {
        // Pede para usar uma das duas impressoras. Bloqueia se ambas estiverem em uso.
        wait(&sem_impressora, p);
        printf("dispatcher => Impressora alocada para o processo %d.\n", p->pid);
    }
    if (p->scanner == 1) {
        // Pede para usar o scanner. Bloqueia se estiver em uso.
        wait(&sem_scanner, p);
        printf("dispatcher => Scanner alocado para o processo %d.\n", p->pid);
    }
    if (p->modem == 1) {
        // Pede para usar o modem. Bloqueia se estiver em uso.
        wait(&sem_modem, p);
        printf("dispatcher => Modem alocado para o processo %d.\n", p->pid);
    }
    if (p->codigo_disco >= 0) {
        // Pede para usar um dos dois discos. Bloqueia se ambos estiverem em uso.
        wait(&sem_disco_sata, p);
        printf("dispatcher => Disco SATA alocado para o processo %d.\n", p->pid);
    }
}

void liberar_recursos(Processo* p) {
    if (p->prioridade == 0) {
        return;
    }
    
    // Devolve os recursos que o processo estava usando
    if (p->impressora >= 0) {
        signal(&sem_impressora);
    }
    if (p->scanner == 1) {
        signal(&sem_scanner);
    }
    if (p->modem == 1) {
        signal(&sem_modem);
    }
    if (p->codigo_disco >= 0) {
        signal(&sem_disco_sata);
    }
    printf("dispatcher => Recursos do processo %d foram liberados.\n", p->pid);
}