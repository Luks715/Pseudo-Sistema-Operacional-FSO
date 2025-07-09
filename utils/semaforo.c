#include <stdio.h>
#include "../include/semaforo.h"
#include "../gerenciador_de_processos/gerenciador_processos.h" 

void iniciar_semaforo(Semaforo* s, int valor_inicial) {
    s->valor = valor_inicial;
    start_queue(&(s->processos_bloqueados));
}

int wait(Semaforo* s, Processo* p_atual) {
    s->valor--;
    if (s->valor < 0) {
        // Recurso não disponível, bloquear o processo.
        printf("dispatcher => Processo %d BLOQUEADO esperando por recurso.\n", p_atual->pid);
        append(&(s->processos_bloqueados), *p_atual);
        return 0; // Sinaliza que o processo foi bloqueado
    }
    
    // Recurso disponível e adquirido com sucesso.
    return 1; // Sinaliza sucesso
}


void signal(Semaforo* s) {
    s->valor++;
    if (s->valor <= 0) {
        // Se o valor ainda é <= 0, significa que havia processos esperando.
        // Acorda o próximo processo da fila de bloqueados.
        if (!queue_empty(&(s->processos_bloqueados))) {
            Processo processo_acordado;
            pop(&(s->processos_bloqueados), &processo_acordado);

            printf("dispatcher => Processo %d ACORDADO e devolvido para a fila de prontos.\n", processo_acordado.pid);

            despachar_processo(processo_acordado);
        }
    }
}