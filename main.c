#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fila.h"
#include "memoria.h"
#include "arquivo.h"
#include "recursoES.h"
#include "disco.h"
#include "diretorio.h"

// Declaração de filas 
Fila fila_global;
Fila fila_tempo_real;
Fila fila_usuario_1;
Fila fila_usuario_2;
Fila fila_usuario_3;

// Memória RAM
Memoria memoria_principal;

// Diretório 
Diretorio diretorio;

// HD
Disco disco;

// Recursos de E/S
Recurso impressoras[2];
Recurso scanner;
Recurso modem;
Recurso discos[2];

// Inicialização de estruturas
void inicializar_filas() {
    fila_tempo_real.inicio = fila_tempo_real.fim = 0;
    fila_usuario_1.inicio = fila_usuario_1.fim = 0;
    fila_usuario_2.inicio = fila_usuario_2.fim = 0;
    fila_usuario_3.inicio = fila_usuario_3.fim = 0;
}

void inicializar_memoria() {
    for (int i = 0; i < TOTAL_BLOCOS; i++) {
        memoria_principal.blocos[i].ocupado = 0;
        memoria_principal.blocos[i].pid = -1;
    }
}

void inicializar_disco() {
    disco.total_blocos = TAM_DISCO;

    // Inicializa blocos físicos
    for (int i = 0; i < TAM_DISCO; i++) {
        disco.blocos[i] = '0';
    }

    // Inicializa o diretório dentro do disco
    disco.diretorio.total_arquivos = 0;
}

void inicializar_recursos() {
    for (int i = 0; i < 2; i++) {
        impressoras[i].ocupado = 0;
        impressoras[i].pid = -1;
        discos[i].ocupado = 0;
        discos[i].pid = -1;
    }
    scanner.ocupado = modem.ocupado = 0;
    scanner.pid = modem.pid = -1;
}


int main(){
    printf("Inicializando o SO.\n");

    inicializar_filas();
    inicializar_memoria();
    inicializar_disco();
    inicializar_recursos();

    printf("Inicialização concluída. Iniciando dispatch.\n");

    dispatcher(&memoria_principal, &disco, &fila_global); // dispatcher() vai usar as filas, memória, disco e recursos
    return 0;
}