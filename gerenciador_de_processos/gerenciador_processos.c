#include <stdio.h>
#include "gerenciador_processos.h"
#include "fila.h"

extern Fila fila_tempo_real;
extern Fila fila_usuario_1;

// Múltiplas Filas com Realimentação.
void despachar_processo(Processo p) {
    // Inicializa o contador de envelhecimento para o processo
    p.ciclos_na_fila = 0;

    // Processos de tempo real (prioridade 0) vão para sua própria fila
    if (p.prioridade == 0) {
        append(&fila_tempo_real, p);
    } else {
        append(&fila_usuario_1, p);
    }
}