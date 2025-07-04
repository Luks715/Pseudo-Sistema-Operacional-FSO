#include <stdio.h>
#include "..\include\recursoES.h"

// Implementação da inicialização dos recursos
void inicializar_recursos(Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]) {
    for (int i = 0; i < 2; i++) {
        impressoras[i].ocupado = 0;
        impressoras[i].pid = -1;
        discos[i].ocupado = 0;
        discos[i].pid = -1;
    }
    scanner->ocupado = 0;
    scanner->pid = -1;
    modem->ocupado = 0;
    modem->pid = -1;
}

// Implementação da alocação de recursos
int alocar_recursos(Processo* p, Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]) {
    if (p->prioridade == 0) {
        return 1; // Sucesso imediato
    }

    // Verifica a disponibilidade de TODOS os recursos necessários ANTES de alocar qualquer um
    if (p->impressora >= 0 && impressoras[p->impressora].ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Impressora %d ja esta em uso.\n", p->pid, p->impressora);
        return 0;
    }
    if (p->scanner == 1 && scanner->ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Scanner ja esta em uso.\n", p->pid);
        return 0;
    }
    if (p->modem == 1 && modem->ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Modem ja esta em uso.\n", p->pid);
        return 0;
    }
    if (p->codigo_disco >= 0 && discos[p->codigo_disco].ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Disco SATA %d ja esta em uso.\n", p->pid, p->codigo_disco);
        return 0;
    }

    // Se todas as verificações passaram, aloca os recursos para o processo
    if (p->impressora >= 0) {
        impressoras[p->impressora].ocupado = 1;
        impressoras[p->impressora].pid = p->pid;
    }
    if (p->scanner == 1) {
        scanner->ocupado = 1;
        scanner->pid = p->pid;
    }
    if (p->modem == 1) {
        modem->ocupado = 1;
        modem->pid = p->pid;
    }
    if (p->codigo_disco >= 0) {
        discos[p->codigo_disco].ocupado = 1;
        discos[p->codigo_disco].pid = p->pid;
    }
    return 1; // Sucesso
}

// Implementação da liberação de recursos
void liberar_recursos(Processo* p, Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2]) {
    if (p->impressora >= 0 && impressoras[p->impressora].pid == p->pid) {
        impressoras[p->impressora].ocupado = 0;
        impressoras[p->impressora].pid = -1;
    }
    if (p->scanner == 1 && scanner->pid == p->pid) {
        scanner->ocupado = 0;
        scanner->pid = -1;
    }
    if (p->modem == 1 && modem->pid == p->pid) {
        modem->ocupado = 0;
        modem->pid = -1;
    }
    if (p->codigo_disco >= 0 && discos[p->codigo_disco].pid == p->pid) {
        discos[p->codigo_disco].ocupado = 0;
        discos[p->codigo_disco].pid = -1;
    }
}