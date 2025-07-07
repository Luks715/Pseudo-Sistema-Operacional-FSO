#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "include\dispatcher.h"
#include "include\arquivo.h"
#include "include\processo.h"
#include "include\operacao.h"
#include "include\memoria.h"
#include "include\disco.h"
#include "include\recursoES.h"

#define MAX_LINHA 100

void imprimir_info_processo(const Processo *p) {
    printf("dispatcher => PID: %d | offset: %d | blocks: %d | priority: %d | time: %d | printers: %s | scanners: %s | modems: %s | drives: %s\n\n",
           p->pid,
           p->offset_mem,
           p->blocos_mem,
           p->prioridade,
           p->tempo_de_processador,
           (p->impressora > 0) ? "true" : "false",
           (p->scanner == 1) ? "true" : "false",
           (p->modem == 1) ? "true" : "false",
           (p->codigo_disco > 0) ? "true" : "false"
    );
}

int dispatcher(Memoria* memoria, Disco* disco, Fila* fila_global,
               Recurso impressoras[2], Recurso* scanner, Recurso* modem, Recurso discos[2])
{
    FILE *arquivo;
    char linha[MAX_LINHA];
    char numero[20];
    int num_idx = 0;

    // Leitura e parsing do arquivo de sistema de arquivos (files.txt)
    arquivo = fopen("files.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo files.txt");
        return 1;
    }

    int counter = 0;
    int segmentos;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        switch (counter) {
            case 0: // Lê a quantidade total de blocos no disco
                for (int i = 0; linha[i] != '\0'; i++) {
                    if (isdigit(linha[i])) {
                        numero[num_idx++] = linha[i];
                    }
                }
                numero[num_idx] = '\0';
                disco->total_blocos = atoi(numero);
                num_idx = 0;
                memset(numero, 0, sizeof(numero));
                counter++;
                break;

            case 1: // Lê a quantidade de segmentos já ocupados
                for (int i = 0; linha[i] != '\0'; i++) {
                    if (isdigit(linha[i])) {
                        numero[num_idx++] = linha[i];
                    }
                }
                numero[num_idx] = '\0';
                segmentos = atoi(numero);
                num_idx = 0;
                memset(numero, 0, sizeof(numero));
                counter++;
                break;
            
            case 2: // Lê os segmentos e preenche o mapa de disco
                 // Este laço precisa ser ajustado para ler as N linhas de segmento
                for (int i = 0; i < segmentos; i++) {
                    // Lógica para ler cada linha de segmento de arquivo
                }
                // Lógica para ler as operações de arquivo restantes
                counter++;
                break;

            default:
                break;
        }
    }
    fclose(arquivo);

    // Leitura e parsing do arquivo de processos (processes.txt)
    arquivo = fopen("processes.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo processes.txt");
        return 1;
    }

    int p_id = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        int campo_processo = 0;
        num_idx = 0;
        memset(numero, 0, sizeof(numero));

        Processo p = {0}; // Inicializa a struct com zeros
        p.pid = p_id;

        for (int i = 0; i <= strlen(linha); i++) {
            if (isdigit(linha[i])) {
                numero[num_idx++] = linha[i];
            } else if (num_idx > 0) {
                numero[num_idx] = '\0';
                int valor = atoi(numero);
                memset(numero, 0, sizeof(numero));
                num_idx = 0;

                switch (campo_processo) {
                    case 0: p.tempo_inicializacao = valor; break;
                    case 1: p.prioridade = valor; break;
                    case 2: p.tempo_de_processador = valor; break;
                    case 3: p.blocos_mem = valor; break;
                    case 4: p.impressora = valor; break;
                    case 5: p.scanner = valor; break;
                    case 6: p.modem = valor; break;
                    case 7: p.codigo_disco = valor; break;
                }
                campo_processo++;
            }
        }

        // --- LÓGICA DE GERENCIAMENTO DE RECURSOS ---
        
        // 1. Tenta alocar os recursos para o processo
        if (alocar_recursos(&p, impressoras, scanner, modem, discos)) {

            imprimir_info_processo(&p);

            // SIMULAÇÃO DA EXECUÇÃO DO PROCESSO
            printf("process %d =>\n", p.pid);
            printf("\tP%d STARTED\n", p.pid);
            for (int i = 1; i <= p.tempo_de_processador; i++) {
                printf("\tP%d instruction %d\n", p.pid, i);
            }
            printf("\tP%d return SIGINT\n\n", p.pid);

            // 2. Libera os recursos DEPOIS que o processo termina
            liberar_recursos(&p, impressoras, scanner, modem, discos);

        } else {
            // FALHA na alocação. O processo é descartado.
            printf("dispatcher => Processo %d foi descartado por falta de recursos.\n\n", p.pid);
        }
        
        p_id++;
    }

    fclose(arquivo);

    return 0;
}