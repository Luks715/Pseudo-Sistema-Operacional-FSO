#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dispatcher.h"
#include "arquivo.h"
#include "processo.h"
#include "operacao.h"
#include "kernel.h"
#include "fila.h"

#define MAX_LINHA 100  // define a quantidade máxima de caracteres na linha

void dispatcher(Memoria* memoria, Disco* disco, Fila* filas[], Kernel* kernel){
    FILE *arquivo;
    char linha[MAX_LINHA]; // pega os 100 primeiros caracteres, se as linhas forem maiores que isso, é possível modificar para pegar a linha até achar '\n'

    // Abre o arquivo Files para leitura
    arquivo = fopen("files.txt", "r");
    if (arquivo == NULL){
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int counter = 0;     // 0 = quantidade blocos, 1 = n segmentos, 2 = arquivos, 3 = operações
    int segmentos;
    int o_idx = 0;

    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        char* valor;

        switch(counter){
            // Pega a primeira linha do arquivo, o número de blocos
            case 0:
                disco->total_blocos = atoi(strtok(linha, ", "));
                counter++;
                break;
            
            // Pega a segunda linha do arquivo, o número de n segmentos
            case 1:
                segmentos = atoi(strtok(linha, ", "));
                counter++;
                break;

            // itera n vezes para pegar cada arquivo
            case 2:
                for (int i = 0; i < segmentos; i++) {
                    Arquivo a; 

                    if(fgets(linha, sizeof(linha), arquivo) != NULL){
                        a.nome = strtok(linha, ", ");
                        a.bloco_inicial = atoi(strtok(NULL, ", "));
                        a.tamanho = atoi(strtok(NULL, ", "));
                    } else {
                        perror("Número de segmentos fornecido incompatível com a quantidade real de segmentos");
                        break;
                    }

                    // Adiciona o arquivo no diretório
                    disco->diretorio.arquivos[i] = a;
                    disco->diretorio.total_arquivos++;

                    // Atualiza o bitmap de blocos utilizados
                    for (int i = 0; i < a.tamanho; i++) {
                        disco->blocos[a.bloco_inicial + i] = a.nome;
                    }
                }

                counter++;
                break;

            // itera até o fim do arquivo para pegar as operações
            default:
                Operacao o;
                
                o.pid = atoi(strtok(linha, ", "));
                o.codigo_op = atoi(strtok(NULL, ", "));
                o.nome_arquivo = strtok(NULL, ", ");

                if(o.codigo_op == 0){
                    o.blocos = atoi(strtok(NULL, ", "));
                }

                // Adiciona a operação no vetor de Operações do disco
                kernel->operacoes[o_idx++] = o;

                break;
        }
    }

    fclose(arquivo); // fecha files.txt

    // Abre o arquivo processes.txt para leitura
    arquivo = fopen("processes.txt", "r");
    if (arquivo == NULL){
        perror("Erro ao abrir o arquivo");
        return 1;
    }


    int p_idx = 0;     // variável de controle para contar os processos prontos
    int p_id = 0;      // variável que armazena os IDs dos processos

    // Itera sobre cada linha do arquivo Processes
    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        Processo p;

        p.pid = p_id++; // atualiza o pid e incrementa p_id
        p.tempo_inicializacao = atoi(strtok(linha, ", "));
        p.prioridade = atoi(strtok(NULL, ", "));
        p.tempo_de_processador = atoi(strtok(NULL, ", "));
        p.blocos_mem = atoi(strtok(NULL, ", "));
        p.impressora = atoi(strtok(NULL, ", "));
        p.scanner = atoi(strtok(NULL, ", "));
        p.modem = atoi(strtok(NULL, ", "));
        p.codigo_disco = atoi(strtok(NULL, ", "));

        // O sistema só deve oferecer suporte para 1000 processos nas filas
        if(p_idx < 999){
            // Chamar o Gerenciador de Memória para alocar o processo na RAM
            int resultado_alocacao = kernel->alocador(&p, memoria);

            if(resultado_alocacao != -1){
                p_idx++;

                // Realiza o Output do processo
                imprimir_info_processo(&p);

                // Adiciona o processo em sua Fila correspondente
                switch(p.prioridade){
                    case 0:
                        append(filas[0], p);     // fila de tempo real
                        break;

                    case 1:
                        append(filas[1], p);     // fila de usuario 1
                        break;

                    case 2:
                        append(filas[2], p);     // fila de usuario 2
                        break;      

                    case 3:
                        append(filas[3], p);     // fila de usuario 3
                        break;
                }

            } else {
                // Não é possível alocar o processo na memória pois já há 1000 processos prontos
                // Adiciona o processo na Fila Global
                append(filas[4], p);
            }
        } else {
            // Adiciona o processo na Fila Global
            append(filas[4], p);
        }
    }

    fclose(arquivo); // fecha processes.txt
}