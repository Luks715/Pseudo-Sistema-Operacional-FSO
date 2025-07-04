#include <stdio.h>
#include <unistd.h>
#include "escalonador.h"
#include "fila.h"
#include "processo.h"
// #include "semaforo.h"       // Descomente quando for usar
// #include "recursoES.h"      // Descomente quando for usar

extern Fila fila_tempo_real;
extern Fila fila_usuario_1;
extern Fila fila_usuario_2;
extern Fila fila_usuario_3;

#define AGING_THRESHOLD 5 

void aging(Fila* fila, Fila* fila_superior) {
    if (is_empty(fila)) {
        return;
    }

    Fila fila_temp;
    start_queue(&fila_temp); // Fila temporária para guardar processos não promovidos

    // Itera sobre a fila original, esvaziando-a
    while (!is_empty(fila)) {
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
    // Simula a execução do processo por 1 quantum de 1ms
    printf("Executando P%d (Prioridade: %d, Tempo Restante: %d)\n", p->pid, p->prioridade, p->tempo_de_processador);
    usleep(1000); // Simula 1ms de trabalho
    p->tempo_de_processador--;

    if (p->tempo_de_processador <= 0) {
        printf("P%d finalizado. return SIGINT\n", p->pid);
        // liberar_recursos(p); // Função para liberar memória e recursos de E/S
    }
}

void rebaixar_processo(Processo* p) {
    printf("Processo %d não terminou. Rebaixando prioridade.\n", p->pid);
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

void escalonar() {
    // O loop continua enquanto houver processos em qualquer fila de prontos.
    while (!is_empty(&fila_tempo_real) ||
           !is_empty(&fila_usuario_1) ||
           !is_empty(&fila_usuario_2) ||
           !is_empty(&fila_usuario_3)) {
        
        Processo p_atual;

        // 1. Processos de Tempo Real (maior prioridade)
        if (!is_empty(&fila_tempo_real)) {
            pop(&fila_tempo_real, &p_atual);
            
            printf("Executando processo de TEMPO REAL P%d até o fim.\n", p_atual.pid);
            while (p_atual.tempo_de_processador > 0) {
                usleep(1000); // Simula 1ms de trabalho
                p_atual.tempo_de_processador--;
            }
            printf("P%d finalizado. return SIGINT\n", p_atual.pid);
            // liberar_recursos(&p_atual);
            continue; // Volta ao início do loop para re-avaliar as filas
        }

        // 2. Processos de Usuário (Múltiplas Filas com Realimentação)
        
        // Aplica o envelhecimento ANTES de escolher o próximo processo de usuário.
        aging(&fila_usuario_2, &fila_usuario_1);
        aging(&fila_usuario_3, &fila_usuario_2);

        // Fila de Prioridade 1
        if (!is_empty(&fila_usuario_1)) {
            pop(&fila_usuario_1, &p_atual);
            executar_processo(&p_atual);
            if (p_atual.tempo_de_processador > 0) {
                rebaixar_processo(&p_atual);
            }
            continue;
        }

        // Fila de Prioridade 2
        if (!is_empty(&fila_usuario_2)) {
            pop(&fila_usuario_2, &p_atual);
            executar_processo(&p_atual);
            if (p_atual.tempo_de_processador > 0) {
                rebaixar_processo(&p_atual);
            }
            continue;
        }

        // Fila de Prioridade 3
        if (!is_empty(&fila_usuario_3)) {
            pop(&fila_usuario_3, &p_atual);
            executar_processo(&p_atual);
            if (p_atual.tempo_de_processador > 0) {
                rebaixar_processo(&p_atual);
            }
            continue;
        }
    }
    printf("\nTodas as filas de processos estão vazias. Escalonador encerrado.\n");
}