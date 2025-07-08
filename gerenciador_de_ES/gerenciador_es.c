#include <stdio.h>
#include "gerenciador_es.h" // CORREÇÃO: Inclui o cabeçalho do próprio módulo

// Implementação da inicialização dos recursos
void inicializar_recursos(RecursosDoSistema* recursos) {
    for (int i = 0; i < 2; i++) {
        recursos->impressoras[i].ocupado = 0;
        recursos->impressoras[i].pid = -1;
        recursos->discos[i].ocupado = 0;
        recursos->discos[i].pid = -1;
    }
    recursos->scanner.ocupado = 0;
    recursos->scanner.pid = -1;
    recursos->modem.ocupado = 0;
    recursos->modem.pid = -1;
}

// Implementação da alocação de recursos
int alocar_recursos(Processo* p, RecursosDoSistema* recursos) {
    if (p->prioridade == 0) {
        return 1; // Sucesso imediato
    }

    // Verifica a disponibilidade de TODOS os recursos necessários ANTES de alocar qualquer um
    if (p->impressora >= 0 && recursos->impressoras[p->impressora].ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Impressora %d ja esta em uso.\n", p->pid, p->impressora);
        return 0;
    }
    if (p->scanner == 1 && recursos->scanner.ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Scanner ja esta em uso.\n", p->pid);
        return 0;
    }
    if (p->modem == 1 && recursos->modem.ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Modem ja esta em uso.\n", p->pid);
        return 0;
    }
    if (p->codigo_disco >= 0 && recursos->discos[p->codigo_disco].ocupado) {
        printf("dispatcher => Falha: Processo %d nao pode ser criado. Disco SATA %d ja esta em uso.\n", p->pid, p->codigo_disco);
        return 0;
    }

    // Se todas as verificações passaram, aloca os recursos para o processo
    if (p->impressora >= 0) {
        recursos->impressoras[p->impressora].ocupado = 1;
        recursos->impressoras[p->impressora].pid = p->pid;
    }
    if (p->scanner == 1) {
        recursos->scanner.ocupado = 1;
        recursos->scanner.pid = p->pid;
    }
    if (p->modem == 1) {
        recursos->modem.ocupado = 1;
        recursos->modem.pid = p->pid;
    }
    if (p->codigo_disco >= 0) {
        recursos->discos[p->codigo_disco].ocupado = 1;
        recursos->discos[p->codigo_disco].pid = p->pid;
    }
    return 1; // Sucesso
}

// Implementação da liberação de recursos
void liberar_recursos(Processo* p, RecursosDoSistema* recursos) {
    if (p->impressora >= 0 && recursos->impressoras[p->impressora].pid == p->pid) {
        recursos->impressoras[p->impressora].ocupado = 0;
        recursos->impressoras[p->impressora].pid = -1;
    }
    if (p->scanner == 1 && recursos->scanner.pid == p->pid) {
        recursos->scanner.ocupado = 0;
        recursos->scanner.pid = -1;
    }
    if (p->modem == 1 && recursos->modem.pid == p->pid) {
        recursos->modem.ocupado = 0;
        recursos->modem.pid = -1;
    }
    if (p->codigo_disco >= 0 && recursos->discos[p->codigo_disco].pid == p->pid) {
        recursos->discos[p->codigo_disco].ocupado = 0;
        recursos->discos[p->codigo_disco].pid = -1;
    }
}