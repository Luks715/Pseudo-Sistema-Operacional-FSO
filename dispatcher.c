#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dispatcher.h"
#include "arquivo.h"
#include "processo.h"
#include "operacao.h"

#define MAX_LINHA 100

void imprimir_info_processo(const Processo *p) {
    printf("dispatcher => PID: %d, offset: %d, blocks: %d, priority: %d, time: %d, printers: %s, scanners: %s, modems: %s, drives: %s\n",
           p->pid,
           p->offset_mem,
           p->blocos_mem,
           p->prioridade,
           p->tempo_de_processador,
           (p->impressora >= 0) ? "true" : "false",
           (p->scanner == 1) ? "true" : "false",
           (p->modem == 1) ? "true" : "false",
           (p->codigo_disco >= 0) ? "true" : "false"
    );
}

int dispatcher(Memoria* memoria, Disco* disco, Fila* fila_global){
    FILE *arquivo;
    char linha[MAX_LINHA]; // pega os 100 primeiros caracteres, se as linhas forem maiores que isso, é possível modificar para pegar a linha até achar '\n'

    // Abre o arquivo Files para leitura
    arquivo = fopen("files.txt", "r");
    if (arquivo == NULL){
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int counter = 0;     // 0 = quantidade blocos, 1 = n segmentos, 2 = arquivos, 3 = operações
    int quantidade_blocos;
    int segmentos;
    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        char numero[20]; // para armazenar cada número da linha
        int num_idx = 0;

        switch(counter){
            // Pega a primeira linha do arquivo, o número de blocos
            case 0: 
                for (int i = 0; linha[i] != '\0'; i++) {
                    if(isdigit(linha[i])){
                        numero[num_idx++] = linha[i];
                    } else {
                        perror("Número de Blocos fornecido não suportado");
                    }
                }
                numero[num_idx] = '\0';
                quantidade_blocos = atoi(numero);
                disco->total_blocos = quantidade_blocos;
                num_idx = 0;
                memset(numero, 0, sizeof(numero)); // caso o próximo número tenha menos dígitos que o anterior, haverá erro, toda a string deve ser resetada
                counter++;
                break;
            
            // Pega a segunda linha do arquivo, o número de n segmentos
            case 1:
                for (int i = 0; linha[i] != '\0'; i++) {
                    if(isdigit(linha[i])){
                        numero[num_idx++] = linha[i];
                    } else {
                        perror("Número de Segmentos fornecido não suportado");
                    }
                }
                numero[num_idx] = '\0';
                segmentos = atoi(numero);
                num_idx = 0;
                memset(numero, 0, sizeof(numero)); // caso o próximo número tenha menos dígitos que o anterior, haverá erro, toda a string deve ser resetada
                counter++;
                break;
            
            // itera n vezes para pegar cada arquivo
            case 2:
                for (int i = 0; i < segmentos; i++) {
                    int campo_arquivo = 0; // 0 = nome, 1 = bloco inicial, 2 = tamanho
                    Arquivo a; 
                    int bloco_inicial; // armazena o bloco inicial do arquivo
                    int tamanho;       // quantidade de blocos ocupados pelo arquivo

                    if(fgets(linha, sizeof(linha), arquivo) != NULL){
                        for(int i = 0; linha[i] != '\0'; i++) {
                            if(isdigit(linha[i])){
                                if(campo_arquivo == 0){
                                    perror("Nome de arquivo não suportado");
                                } else {
                                    numero[num_idx++] = linha[i];
                                }
                            } else if(isalpha(linha[i])){
                                if(campo_arquivo == 0){
                                    a.nome = linha[i];
                                    campo_arquivo++;
                                } else {
                                    perror("Número fornecido não suportado");
                                }
                            } else {
                                if(num_idx >0){
                                    numero[num_idx] = '\0'; // finaliza string

                                    switch(campo_arquivo){
                                        case 1: a.bloco_inicial = atoi(numero); break;
                                        case 2: a.tamanho = atoi(numero); break;
                                    }
                                    memset(numero, 0, sizeof(numero)); // caso o próximo número tenha menos dígitos que o anterior, haverá erro, toda a string deve ser resetada
                                    num_idx = 0;
                                    campo_arquivo++;
                                }
                            }
                        }
                    } else {
                        perror("Não foi fornecido nenhum segmento");
                    }

                    // Atualiza o bitmap de blocos utilizados
                    for (int i = 0; i < a.tamanho; i++) {
                        disco->blocos[a.bloco_inicial + i] = a.nome;
                    }

                    // Adiciona o arquivo no diretório
                    disco->diretorio.arquivos[i] = a;
                    disco->diretorio.total_arquivos++;
                }
                counter++;
                break;
            
            // itera até o fim do arquivo para pegar as operações
            default:
                Operacao o;
                int campo_operacao = 0;

                for(int i = 0; linha[i] != '\0'; i++) {
                    if (isdigit(linha[i])) {
                        if(campo_operacao == 2){
                            perror("Nome de arquivo não suportado");
                        } else {
                            numero[num_idx++] = linha[i];
                        }

                    } else if(isalpha(linha[i])){
                        if(campo_operacao == 2){
                            o.nome_arquivo = linha[i];
                            campo_operacao++;
                        } else {
                            perror("Número fornecido não suportado");
                        }
                    } else {
                        if (num_idx > 0) {
                            numero[num_idx] = '\0'; // finaliza string

                            switch(campo_operacao){
                                case 0: o.pid = atoi(numero); break;
                                case 1: o.codigo_op = atoi(numero); break;
                                case 3: o.blocos = atoi(numero); break;
                            }

                            memset(numero, 0, sizeof(numero)); // caso o próximo número tenha menos dígitos que o anterior, haverá erro, toda a string deve ser resetada
                            num_idx = 0;
                            campo_operacao++;
                        }
                    }
                }
                // código para armazenar as operações na RAM/no disco
            break;
        }
    }

    fclose(arquivo); // fecha files.txt

    // Abre o arquivo Processes para leitura
    arquivo = fopen("processes.txt", "r");
    if (arquivo == NULL){
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Itera sobre cada linha do arquivo Processes
    int p_idx = 0;
    int p_id = 0;
    while(fgets(linha, sizeof(linha), arquivo) != NULL){
        int campo_processo = 0;
        char numero[20]; // para armazenar cada número da linha
        int num_idx = 0;

        Processo p;
        p.pid = p_id++; // atualiza o pid e incrementa p_id

        for (int i = 0; linha[i] != '\0'; i++) {
            if (isdigit(linha[i])) {
                numero[num_idx++] = linha[i];
            } else {
                if (num_idx > 0) {
                    numero[num_idx] = '\0'; // finaliza string
                    int valor = atoi(numero);
                    memset(numero, 0, sizeof(numero)); // caso o próximo número tenha menos dígitos que o anterior, haverá erro, toda a string deve ser resetada
                    num_idx = 0;
    
                    // Atribui o valor ao campo do Processo
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
        }
        // Código para adicionar o processo na lista global
        fila_global->processos[p_idx++] = p;

        // Chamar o Gerenciador de Memória para alocar o processo na RAM
        p.offset_mem = alocar_processo(p); // A função alocar processo ainda não foi feita

        // Realiza o Output do processo
        imprimir_info_processo(&p);
    }
}
