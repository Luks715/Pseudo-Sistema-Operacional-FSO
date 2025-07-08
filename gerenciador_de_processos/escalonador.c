#include <stdio.h>
#include <unistd.h>
//#include <time.h>

#include "escalonador.h"
#include "../include/fila.h"
#include "../include/processo.h"
#include "../include/semaforo.h"      
#include "../include/recursoES.h"
#include "../gerenciador_de_ES/gerenciador_es.h" 
#include "../gerenciador_de_memoria/alocador.h"
#include "../include/dispatcher.h"

extern Memoria RAM;
extern Fila fila_tempo_real;
extern Fila fila_usuario_1;
extern Fila fila_usuario_2;
extern Fila fila_usuario_3;

#define AGING_THRESHOLD 5 

void aging(Fila* fila, Fila* fila_superior) {
    if (queue_empty(fila)) {
        return;
    }

    Fila fila_temp;
    start_queue(&fila_temp); // Fila temporária para guardar processos não promovidos

    // Itera sobre a fila original, esvaziando-a
    while (!queue_empty(fila)) {
        Processo p;
        pop(fila, &p); // Retira o processo da fila original

        p.ciclos_na_fila++;

        if (p.ciclos_na_fila >= AGING_THRESHOLD) {
            // Promove o processo
            printf("Aging: Processo %d promovido da prioridade %d para %d.\n",
                   p.pid, p.prioridade, p.prioridade - 1);
            
            p.prioridade--;
            p.ciclos_na_fila = 0; // Reseta o contador de envelhecimento
            append(fila_superior, p); // Adiciona na fila de prioridade maior
        } else {
            // Mantém o processo na mesma prioridade
            append(&fila_temp, p); // Adiciona na fila temporária
        }
    }
    *fila = fila_temp;
}


void executar_processo(Processo* p) {
    executar_operacoes_de_arquivo(p); 

    printf("Executando P%d (Prioridade: %d, Tempo Restante: %d)\n", p->pid, p->prioridade, p->tempo_de_processador);
    usleep(1000); 
    p->tempo_de_processador--;

    if (p->tempo_de_processador <= 0) {
        printf("P%d finalizado. return SIGINT\n", p->pid);
        
        liberar_recursos(p);
        liberar_memoria(p, &RAM);
    }
}
void rebaixar_processo(Processo* p) {
    printf("Processo %d nao terminou. Rebaixando prioridade.\n", p->pid);
    p->ciclos_na_fila = 0; // Reseta o contador ao mudar de fila

    if (p->prioridade == 1) {
        p->prioridade = 2;
        append(&fila_usuario_2, *p);
    } else if (p->prioridade == 2) {
        p->prioridade = 3;
        append(&fila_usuario_3, *p);
    } else { 
        append(&fila_usuario_3, *p);
    }
}

static int processar_fila_usuario(Fila* fila, Processo* p_atual) {
    if (!queue_empty(fila)) { 
        pop(fila, p_atual);
        executar_processo(p_atual);
        if (p_atual->tempo_de_processador > 0) {
            rebaixar_processo(p_atual);
        }
        return 1; // Indica que um processo foi tratado
    }
    return 0; // Indica que a fila estava vazia
}


void escalonar() {
    // O loop continua enquanto houver processos em qualquer fila de prontos.
    while (!queue_empty(&fila_tempo_real) ||
           !queue_empty(&fila_usuario_1) ||
           !queue_empty(&fila_usuario_2) ||
           !queue_empty(&fila_usuario_3)) {
        
        Processo p_atual;

        // 1. Processos de Tempo Real (maior prioridade)
        if (!queue_empty(&fila_tempo_real)) {
            pop(&fila_tempo_real, &p_atual);
            
            printf("Executando processo de TEMPO REAL P%d ate o fim.\n", p_atual.pid);
            while (p_atual.tempo_de_processador > 0) {
                executar_operacoes_de_arquivo(&p_atual); // Executa operações de arquivo pendentes
                usleep(1000); // Simula 1ms de trabalho
                p_atual.tempo_de_processador--;
            }
            printf("P%d finalizado. return SIGINT\n", p_atual.pid);
            
            liberar_recursos(&p_atual);
            liberar_memoria(&p_atual, &RAM);
            continue; // Volta ao início do loop para re-avaliar as filas
        }

        // 2. Processos de Usuário (Múltiplas Filas com Realimentação)
        
        // Aplica o envelhecimento ANTES de escolher o próximo processo de usuário.
        aging(&fila_usuario_2, &fila_usuario_1);
        aging(&fila_usuario_3, &fila_usuario_2);

        if (processar_fila_usuario(&fila_usuario_1, &p_atual)) {
            continue;
        }
        if (processar_fila_usuario(&fila_usuario_2, &p_atual)) {
            continue;
        }
        if (processar_fila_usuario(&fila_usuario_3, &p_atual)) {
            continue;
        }
    }
    printf("\nTodas as filas de processos estao vazias. Escalonador encerrado.\n");
}