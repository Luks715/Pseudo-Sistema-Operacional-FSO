#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Inclusão dos Módulos do Sistema ---
#include "include/fila.h"
#include "include/memoria.h"
#include "include/disco.h"
#include "include/kernel.h"
#include "include/semaforo.h" 

#include "include/dispatcher.h"
#include "gerenciador_de_memoria/alocador.h"
#include "gerenciador_de_arquivos/sistema_arquivos.h"
#include "gerenciador_de_processos/gerenciador_processos.h"
#include "gerenciador_de_processos/escalonador.h"

// --- Definição das Variáveis Globais do Sistema ---

// Kernel e Hardware Simulado
Kernel kernel;
Memoria RAM;
Disco HD;

// Filas de Processos
Fila fila_global;
Fila fila_tempo_real;
Fila fila_usuario_1;
Fila fila_usuario_2;
Fila fila_usuario_3;

// Semáforos para Controle de Recursos de E/S
Semaforo sem_impressora;
Semaforo sem_scanner;
Semaforo sem_modem;
Semaforo sem_disco_sata;


// --- Funções de Inicialização ---

void inicializar_kernel(){
    // A implementação desta função deve ser atualizada para a struct Kernel final
    kernel.alocador = alocador;
    // kernel.sistema_arquivos = sistema_arquivos; // A forma de chamar mudou
}

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
    for (int i = 0; i < TAM_DISCO; i++) {
        HD.blocos[i] = '0';
    }

    // Inicializa o diretório de arquivos
    for (int i = 0; i < MAX_ARQUIVOS; i++) {
        HD.diretorio.arquivos[i].slot_valido = 0;
        HD.diretorio.arquivos[i].nome = '0';       // Ou outro valor padrão
        HD.diretorio.arquivos[i].pid_dono = -1;  // Valor inválido
        HD.diretorio.arquivos[i].bloco_inicial = -1;
        HD.diretorio.arquivos[i].tamanho = 0;
    }
    
    HD.diretorio.total_arquivos = 0;
}

void imprimir_mapa_disco(Disco* disco) {
    printf("\nEstado do disco: ");
    for (int i = 0; i < disco->total_blocos; i++) {
        printf("%c", disco->blocos[i]);
    }
    printf("\n");
}


// --- Ponto de Entrada Principal ---

int main(int argc, char *argv[]){
    // 1. Validação dos argumentos de linha de comando
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_processos> <arquivo_disco>\n", argv[0]);
        return 1;
    }

    printf("Inicializando o pseudo-SO...\n");

    // 2. Inicializa todos os componentes do sistema
    inicializar_kernel();
    inicializar_filas();
    inicializar_memoria();
    inicializar_disco();

    // 3. Inicializa os semáforos com a quantidade de cada recurso
    iniciar_semaforo(&sem_impressora, 2);
    iniciar_semaforo(&sem_scanner, 1);
    iniciar_semaforo(&sem_modem, 1);
    iniciar_semaforo(&sem_disco_sata, 2);

    printf("Inicializacao concluida.\n\n");

    // 4. Chama o dispatcher, que agora gerencia todo o ciclo da simulação
    // Passa os nomes dos arquivos para o dispatcher ler
    dispatcher(argv[1], argv[2]);

    // Chama a função que imprime o estado final do disco
    imprimir_mapa_disco(&HD);

    printf("\nSimulacao concluida. O sistema sera encerrado.\n");

    return 0;
}