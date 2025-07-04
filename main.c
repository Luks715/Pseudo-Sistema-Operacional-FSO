// VERSÃO LIMPA: Código revisado para remover caracteres especiais (espaços não-padrão).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include\fila.h"
#include "include\memoria.h"
#include "include\arquivo.h"
#include "include\recursoES.h"
#include "include\disco.h"
#include "include\diretorio.h"
#include "include\kernel.h"

#include "include\dispatcher.h"
#include "gerenciador_de_memoria\alocador.h"
#include "gerenciador_de_arquivos\sistema_arquivos.h"

#include "gerenciador_de_processos\gerenciador_processos.h"
#include "gerenciador_de_processos\escalonador.h"
#include "include\semaforo.h" 

// Declaração do Kernel
Kernel kernel;
#include "include\dispatcher.h"

// Declaração de filas
Fila fila_global;      // Guarda todas as structs processos
Fila fila_pronto;      // Guarda todos os processos que estão em memória
Fila fila_tempo_real;  // Processos de prioridade 0
Fila fila_usuario_1;   // Processos de prioridade 1
Fila fila_usuario_2;   // Processos de prioridade 2
Fila fila_usuario_3;   // Processos de prioridade 3

// Variáveis globais para uso dos semáforos
Semaforo sem_impressora;
Semaforo sem_scanner;
Semaforo sem_modem;
Semaforo sem_disco;

Fila* filas[] = { &fila_tempo_real, &fila_usuario_1, &fila_usuario_2, &fila_usuario_3, &fila_global};

// Memória principal
Memoria RAM;

// Diretório
Diretorio diretorio;

// Disco
Disco HD;

// Recursos de E/S
Recurso impressoras[2];
Recurso scanner;
Recurso modem;
Recurso discos[2];

// Inicialização de Kernel
void inicializar_kernel(){
    kernel.alocador = alocador;
    kernel.sistema_arquivos = sistema_arquivos;
}

// Inicialização de estruturas
void inicializar_filas() {
    start_queue(&fila_global);
    start_queue(&fila_tempo_real);
    start_queue(&fila_usuario_1);
    start_queue(&fila_usuario_2);
    start_queue(&fila_usuario_3);
}

void inicializar_memoria() {
    for (int i = 0; i < TOTAL_BLOCOS; i++) {
        RAM.blocos[i].ocupado = 0;
        RAM.blocos[i].pid = -1;
    }
}

void inicializar_disco() {
    HD.total_blocos = TAM_DISCO;

    // Inicializa blocos físicos
    for (int i = 0; i < TAM_DISCO; i++) {
        HD.blocos[i] = '0';
    }

    // Inicializa o diretório dentro do disco
    HD.diretorio.total_arquivos = 0;
}

int main(){
    printf("Inicializando o SO.\n");

    inicializar_kernel();
    inicializar_filas();
    inicializar_gerenciador();
    inicializar_memoria();
    inicializar_disco();
    inicializar_recursos(impressoras, &scanner, &modem, discos);

    printf("Inicializacao concluida. Iniciando dispatch.\n\n");

    dispatcher(&RAM, &HD, filas, &kernel,
           impressoras, &scanner, &modem, discos); // dispatcher() vai usar as filas, memória, disco e recursos

    printf("Iniciando escalonador.\n");
    escalonar(); // executa os processos

    printf("Execução de todos os processos concluída.\n");
    return 0;
}