#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include "include/dispatcher.h"
#include "include/processo.h"
#include "include/fila.h"
#include "gerenciador_de_processos/gerenciador_processos.h"
#include "gerenciador_de_memoria/alocador.h"
#include "gerenciador_de_ES/gerenciador_es.h"
#include "gerenciador_de_processos/escalonador.h"
#include "include/kernel.h"
#include "include/disco.h"
#include "include/operacao.h"
#include "include/semaforo.h"
#include "gerenciador_de_arquivos/sistema_arquivos.h"

extern Memoria RAM;
extern Disco HD;
extern Kernel kernel;
extern Fila fila_global, fila_tempo_real, fila_usuario_1, fila_usuario_2, fila_usuario_3;
extern Semaforo sem_impressora, sem_scanner, sem_modem, sem_disco_sata;

#define MAX_PROCESSOS_ARQUIVO 1000
#define MAX_LINHA 256

// --- Funções Auxiliares ---

void imprimir_info_processo(Processo* p) {
    printf("dispatcher =>\n");
    printf("    PID: %d\n", p->pid);
    printf("    offset: %d\n", p->offset_mem);
    printf("    blocks: %d\n", p->blocos_mem);
    printf("    priority: %d\n", p->prioridade);
    printf("    time: %d\n", p->tempo_de_processador);
    printf("    printers: %d\n", p->impressora);
    printf("    scanners: %d\n", p->scanner);
    printf("    modems: %d\n", p->modem);
    printf("    drives: %d\n\n", p->codigo_disco);
}

int ler_arquivo_disco(const char* nome_arquivo, Disco* disco, Kernel* krnl) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de disco (files.txt)");
        return 0;
    }

    char linha[MAX_LINHA];
    int num_segmentos = 0;
    int operacoes_idx = 0;

    // Linha 1: Total de blocos no disco
    if (fgets(linha, sizeof(linha), arquivo)) {
        disco->total_blocos = atoi(linha);
    }

    // Linha 2: Quantidade de segmentos ocupados
    if (fgets(linha, sizeof(linha), arquivo)) {
        num_segmentos = atoi(linha);
    }

    // Linhas 3 a N+2: Leitura dos segmentos iniciais
    for (int i = 0; i < num_segmentos; i++) {
        if (fgets(linha, sizeof(linha), arquivo)) {
            Arquivo a;
            a.nome = strtok(linha, ", ")[0];
            a.bloco_inicial = atoi(strtok(NULL, ", "));
            a.tamanho = atoi(strtok(NULL, ", "));
            a.pid_dono = -1; // Arquivos iniciais não têm dono

            disco->diretorio.arquivos[disco->diretorio.total_arquivos++] = a;

            for (int j = 0; j < a.tamanho; j++) {
                disco->blocos[a.bloco_inicial + j] = a.nome;
            }
        }
    }

    // Resto do arquivo: Operações de arquivo a serem feitas pelos processos
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (operacoes_idx >= TAM_OP) break;
        Operacao o;
        o.pid = atoi(strtok(linha, ", "));
        o.codigo_op = atoi(strtok(NULL, ", "));
        o.nome_arquivo = strtok(NULL, ", ")[0];

        if (o.codigo_op == 0) { // Se for criação, pega o tamanho
            o.blocos = atoi(strtok(NULL, ", "));
        } else {
            o.blocos = 0;
        }
        krnl->operacoes[operacoes_idx++] = o;
    }
    krnl->num_operacoes = operacoes_idx; // Agora este campo existe no kernel.h

    fclose(arquivo);
    return 1;
}

int ler_arquivo_processos(const char* nome_arquivo, Processo processos_pendentes[], int* total_processos) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de processos");
        return 0;
    }

    char linha[MAX_LINHA];
    *total_processos = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (*total_processos >= MAX_PROCESSOS_ARQUIVO) break;
        Processo* p = &processos_pendentes[*total_processos];
        
        p->pid = *total_processos;
        p->tempo_inicializacao = atoi(strtok(linha, ", "));
        p->prioridade = atoi(strtok(NULL, ", "));
        p->tempo_de_processador = atoi(strtok(NULL, ", "));
        p->blocos_mem = atoi(strtok(NULL, ", "));
        p->impressora = atoi(strtok(NULL, ", "));
        p->scanner = atoi(strtok(NULL, ", "));
        p->modem = atoi(strtok(NULL, ", "));
        p->codigo_disco = atoi(strtok(NULL, ", "));
        
        (*total_processos)++;
    }

    fclose(arquivo);
    return 1;
}

int sistema_esta_ativo(int processos_despachados, int total_processos_arquivo) {
    if (processos_despachados < total_processos_arquivo) return 1;
    if (!queue_empty(&fila_tempo_real) || !queue_empty(&fila_usuario_1) || !queue_empty(&fila_usuario_2) || !queue_empty(&fila_usuario_3)) return 1;
    if (!queue_empty(&sem_impressora.processos_bloqueados) || !queue_empty(&sem_scanner.processos_bloqueados) || !queue_empty(&sem_modem.processos_bloqueados) || !queue_empty(&sem_disco_sata.processos_bloqueados)) return 1;
    return 0;
}

void dispatcher(const char* arq_proc, const char* arq_files) {
    Processo processos_pendentes[MAX_PROCESSOS_ARQUIVO];
    int total_processos_no_arquivo;
    int processos_ja_avaliados = 0;

    // Usa as globais 'HD' e 'kernel' que agora estão visíveis via 'extern'
    ler_arquivo_disco(arq_files, &HD, &kernel);
    ler_arquivo_processos(arq_proc, processos_pendentes, &total_processos_no_arquivo);
    
    int tempo_atual = 0;
    
    while (sistema_esta_ativo(processos_ja_avaliados, total_processos_no_arquivo)) {
        printf("\n--- TEMPO: %d ---\n", tempo_atual);

        for (int i = 0; i < total_processos_no_arquivo; i++) {
            if (processos_pendentes[i].tempo_inicializacao == tempo_atual) {
                printf("dispatcher => Tentando despachar processo %d\n", processos_pendentes[i].pid);
                processos_ja_avaliados++;

                int offset = alocador(&processos_pendentes[i], &RAM);
                if (offset != -1) {
                    processos_pendentes[i].offset_mem = offset;
                    
                    // A alocação de recursos com semáforos é bloqueante,
                    // mas em nossa simulação de thread única, assumimos que o dispatcher
                    // irá esperar aqui se necessário.
                    alocar_recursos(&processos_pendentes[i]);
                    
                    imprimir_info_processo(&processos_pendentes[i]);
                    despachar_processo(processos_pendentes[i]);
                } else {
                    fprintf(stderr, "dispatcher => Falha: Processo %d nao pode ser alocado por falta de memoria.\n", processos_pendentes[i].pid);
                }
            }
        }
        
        escalonar(); 
        
        tempo_atual++;
        usleep(100000); // 0.1 segundo de pausa para legibilidade
    }
}


// Esta função procura e executa a primeira operação de arquivo pendente para um dado processo.
void executar_operacoes_de_arquivo(Processo* processo_atual) {
    // Itera pela lista de operações que foi lida do files.txt
    for (int i = 0; i < kernel.num_operacoes; i++) {
        // Verifica se a operação pertence ao processo que está executando agora
        if (kernel.operacoes[i].pid == processo_atual->pid) {
            
            Operacao* op = &kernel.operacoes[i];
            printf("Sistema de Arquivos => Processo %d executando operacao...\n", processo_atual->pid);

            // Chama a função do sistema de arquivos para realizar a operação
            int resultado = sistema_arquivos(op->codigo_op, op->nome_arquivo, op->blocos, &HD, processo_atual);

            int offset_criacao = -1;
            if (resultado > 0 && op->codigo_op == 0) {
                 Arquivo* arq_criado = buscar_arquivo(op->nome_arquivo, &HD);
                 if (arq_criado) offset_criacao = arq_criado->bloco_inicial;
            }

            // Imprime o resultado da operação com base no código de retorno
            switch (resultado) {
                case 1: // Sucesso na criação
                    printf("Operacao %d => Sucesso\n", i);
                    printf("O processo %d criou o arquivo %c (blocos %d a %d).\n\n", processo_atual->pid, op->nome_arquivo, offset_criacao, offset_criacao + op->blocos - 1);
                    break;
                case 2: // Sucesso na deleção
                    printf("Operacao %d => Sucesso\n", i);
                    printf("O processo %d deletou o arquivo %c.\n\n", processo_atual->pid, op->nome_arquivo);
                    break;
                case -1: // Falha por falta de espaço
                    printf("Operacao %d => Falha\n", i);
                    printf("O processo %d nao pode criar o arquivo %c (falta de espaco).\n\n", processo_atual->pid, op->nome_arquivo);
                    break;
                case -2: // Falha porque o arquivo já existe
                    printf("Operacao %d => Falha\n", i);
                    printf("O processo %d nao pode criar o arquivo %c (ja existe).\n\n", processo_atual->pid, op->nome_arquivo);
                    break;
                case -3: // Falha por falta de permissão
                    printf("Operacao %d => Falha\n", i);
                    printf("O processo %d nao pode deletar o arquivo %c (falta de permissao).\n\n", processo_atual->pid, op->nome_arquivo);
                    break;
                case -4: // Falha porque o arquivo não existe para ser deletado
                    printf("Operacao %d => Falha\n", i);
                    printf("O processo %d nao pode deletar o arquivo %c (nao existe).\n\n", processo_atual->pid, op->nome_arquivo);
                    break;
                default:
                    printf("Operacao %d => Falha\n", i);
                    printf("Ocorreu um erro desconhecido (%d).\n\n", resultado);
            }

            // Marca a operação como concluída para não ser executada novamente
            op->pid = -1;

            // Para a busca, pois um processo geralmente faz uma operação de E/S por vez
            return; 
        }
    }
}