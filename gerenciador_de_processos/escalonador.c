#include <stdio.h>
#include <stdlib.h>

#include "fila.h"

// FIFO DEVE SER UTILIZADO PARA A FILA DE TEMPO REAL

// OUTRAS FILAS DEVEM UTILIZAR ROUND ROBIN COM QUANTUM DE 1 MILISEGUNDO
// PROCESSOS DE OUTRAS FILAS DEVEM PODER SOFRER AGING E/OU DECAYING

void escalonador(Fila* fila_tempo_real, Fila* fila_usuario_1, Fila* fila_usuario_2, Fila* fila_usuario_3){
    
}