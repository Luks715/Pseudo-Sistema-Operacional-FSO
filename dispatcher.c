#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// É uma boa prática manter os includes de headers do seu próprio projeto juntos
#include "include/dispatcher.h"
#include "include/processo.h"
#include "include/memoria.h"
#include "include/disco.h"
#include "include/recursoES.h"
#include "include/fila.h"
// Você precisará criar e implementar as funções deste módulo
#include "gerenciador_de_processos/gerenciador_processos.h"

#define MAX_PROCESSOS 1000

// Função auxiliar para carregar os processos do arquivo para uma lista
int carregar_processos_para_lista(Processo lista_chegada[MAX_PROCESSOS]) {
    FILE *arquivo = fopen("processes.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo processes.txt");
        return 0;
    }

    char linha[100];
    int total_processos = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL && total_processos < MAX_PROCESSOS) {
        Processo p = {0}; // Inicializa o processo com zeros
        p.pid = total_processos;
        
        // Usando strtok que é mais conciso, como em partes do seu código
        p.tempo_inicializacao = atoi(strtok(linha, ", "));
        p.prioridade = atoi(strtok(NULL, ", "));
        p.tempo_de_processador = atoi(strtok(NULL, ", "));
        p.blocos_mem = atoi(strtok(NULL, ", "));
        p.impressora = atoi(strtok(NULL, ", "));
        p.scanner = atoi(strtok(NULL, ", "));
        p.modem = atoi(strtok(NULL, ", "));
        p.codigo_disco = atoi(strtok(NULL, ", "));
        
        // Inicializa outros campos importantes
        p.offset_mem = -1; // -1 significa que ainda não está na memória
        p.tempo_restante = p.tempo_de_processador;

        lista_chegada[total_processos++] = p;
    }

    fclose(arquivo);
    return total_processos;
}


// O dispatcher agora se torna o loop principal do Kernel/SO
void dispatcher_main_loop(Memoria* memoria, Disco* disco, Fila filas[],
                          Recurso impressoras[], Recurso* scanner, Recurso* modem, Recurso discos[]) {
    
    // --- FASE 1: INICIALIZAÇÃO E CARGA ---
    
    Processo lista_de_chegada[MAX_PROCESSOS];
    int total_processos = carregar_processos_para_lista(lista_de_chegada);
    int processos_admitidos = 0;

    int tempo_atual = 0;
    Processo* processo_na_cpu = NULL;

    printf("--- SIMULAÇÃO DO PSEUDO-SO INICIADA ---\n\n");

    // --- FASE 2: LOOP PRINCIPAL DE SIMULAÇÃO (O RELÓGIO DO SISTEMA) ---
    while (true) {
        // 1. ADMISSÃO DE NOVOS PROCESSOS
        // Verifica se algum processo da lista de chegada deve entrar no sistema neste instante de tempo
        for (int i = 0; i < total_processos; i++) {
            if (lista_de_chegada[i].tempo_inicializacao == tempo_atual) {
                printf("Tempo %d: Processo %d chegou ao sistema.\n", tempo_atual, lista_de_chegada[i].pid);
                // Tenta alocar memória para o novo processo
                lista_de_chegada[i].offset_mem = alocar_memoria(&lista_de_chegada[i], memoria);
                
                if (lista_de_chegada[i].offset_mem != -1) {
                    // Se conseguiu memória, o processo é admitido e vai para a fila de prontos
                    printf("Processo %d alocou memoria com sucesso (offset: %d).\n", lista_de_chegada[i].pid, lista_de_chegada[i].offset_mem);
                    despachar_processo(&lista_de_chegada[i], filas); // Função que coloca o processo na fila correta (TR ou User)
                    processos_admitidos++;
                } else {
                    // Se não conseguiu memória, você pode tentar de novo no próximo ciclo ou descartar.
                    // Por enquanto, vamos apenas avisar.
                    printf("Falha de memoria para o processo %d. Tentando novamente no proximo ciclo.\n", lista_de_chegada[i].pid);
                    lista_de_chegada[i].tempo_inicializacao++; // Tenta de novo no próximo "segundo"
                }
            }
        }

        // 2. ESCALONAMENTO
        // Se a CPU está livre, o escalonador escolhe o próximo processo
        if (processo_na_cpu == NULL) {
            processo_na_cpu = escalonar_proximo_processo(filas);
            if (processo_na_cpu != NULL) {
                // Se um processo foi escolhido, agora tentamos alocar os recursos de E/S para ele
                if (!alocar_recursos(processo_na_cpu, impressoras, scanner, modem, discos)) {
                    printf("Processo %d nao conseguiu alocar E/S, voltando para a fila de prontos.\n", processo_na_cpu->pid);
                    // O processo volta para a fila e a CPU fica ociosa neste ciclo
                    despachar_processo(processo_na_cpu, filas); // Devolve para a fila
                    processo_na_cpu = NULL; 
                } else {
                     printf("Tempo %d: Processo %d INICIOU EXECUCAO.\n", tempo_atual, processo_na_cpu->pid);
                     imprimir_info_processo(processo_na_cpu);
                }
            }
        }
        
        // 3. EXECUÇÃO
        if (processo_na_cpu != NULL) {
            processo_na_cpu->tempo_restante--;
            
            // 4. VERIFICAÇÃO DE TÉRMINO OU PREEMPÇÃO
            if (processo_na_cpu->tempo_restante <= 0) {
                // Processo terminou
                printf("Tempo %d: Processo %d TERMINOU.\n", tempo_atual, processo_na_cpu->pid);
                liberar_memoria(processo_na_cpu, memoria);
                liberar_recursos(processo_na_cpu, impressoras, scanner, modem, discos);
                processo_na_cpu = NULL; // CPU fica livre
            } 
            // Adicionar lógica de preempção por quantum e aging aqui
        } else {
            printf("Tempo %d: CPU OCIOSA.\n", tempo_atual);
        }

        // CRITÉRIO DE PARADA: se todos os processos foram admitidos e terminaram
        // (Esta lógica precisa ser melhorada para contar os processos terminados)
        if (processos_admitidos == total_processos && todas_as_filas_estao_vazias() && processo_na_cpu == NULL) {
            printf("\n--- TODOS OS PROCESSOS FORAM CONCLUIDOS ---\n");
            break;
        }

        tempo_atual++;
        // Sleep(100); // Para visualizar a simulação passo a passo
    }

    // Lógica para executar as operações de arquivo pode vir aqui no final
}