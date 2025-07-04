#include "semaforo.h"
#include "gerenciador_processos.h" 

extern Fila fila_tempo_real;
extern Fila fila_usuario_1;

void iniciar_semaforo(Semaforo* s, int valor_inicial) {
    s->valor = valor_inicial;
    start_queue(&(s->processos_bloqueados)); // Inicializa a fila de bloqueados
}

void wait(Semaforo* s, Processo* p_atual, Fila* fila_prontos_origem) {
    s->valor--;
    if (s->valor < 0) {
        // Recurso não disponível, o processo deve ser bloqueado.
        printf("Processo %d bloqueado esperando pelo recurso.\n", p_atual->pid);
        append(&(s->processos_bloqueados), *p_atual);
    } 
}


// Libera o semáforo e acorda um processo que estava esperando
void signal(Semaforo* s) {
    s->valor++;
    if (s->valor <= 0) {
        // Acorda um processo
        if (!queue_empty(&(s->processos_bloqueados))) {
            Processo processo_acordado;
            pop(&(s->processos_bloqueados), &processo_acordado);

            printf("Processo %d acordado e devolvido para a fila de prontos.\n", processo_acordado.pid);

            despachar_processo(processo_acordado);
        }
    }
}