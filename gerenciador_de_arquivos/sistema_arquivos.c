#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// --- Inclusão dos Módulos do Sistema ---
#include "../include/arquivo.h"
#include "sistema_arquivos.h"

int first_fit_disco(int tamanho, Disco* disco) {
    int blocos_livres = 0;
    int endereco_inicial = -1;

    for (int i = 0; i < disco->total_blocos; i++) {
        if (disco->blocos[i] == '0') {
            if (blocos_livres == 0) endereco_inicial = i;
            blocos_livres++;
        } else {
            blocos_livres = 0;
            endereco_inicial = -1;
        }

        if (blocos_livres >= tamanho) {
            return endereco_inicial;
        }
    }

    return -1; // Nenhum espaço encontrado
}

Arquivo* buscar_arquivo(const char nome_arquivo, Disco* disco, int* index_arquivo) {
    int arquivos_vistos = 0;

    for (int i = 0; i < MAX_ARQUIVOS; i++) {
        if (disco->diretorio.arquivos[i].slot_valido == 1) {
            arquivos_vistos++;

            if (disco->diretorio.arquivos[i].nome == nome_arquivo) {
                *index_arquivo = i;
                return &(disco->diretorio.arquivos[i]);
            }

            if (arquivos_vistos == disco->diretorio.total_arquivos) {
                return NULL;
            }
        }
    }
    return NULL; // Não encontrado
}

void adicionar_no_diretorio(Arquivo* arquivo, Disco* disco) {
    for(int i = 0; i < MAX_ARQUIVOS; i++){
        if(disco->diretorio.arquivos[i].slot_valido == 0){
            disco->diretorio.arquivos[i] = *arquivo;
            disco->diretorio.total_arquivos++;
            break;
        }
    }
}

void remover_do_diretorio(int index, Disco* disco) {
    disco->diretorio.arquivos[index].slot_valido = 0;
    disco->diretorio.total_arquivos--;
}


int sistema_arquivos(int operacao, char nome_arquivo, int tamanho_bloco, Disco* disco, Processo* processo) {
    int index_arquivo = -1;
    Arquivo* arquivo_buscado = buscar_arquivo(nome_arquivo, disco, &index_arquivo);

    switch (operacao) {
        case 0: // Operação de CRIAR arquivo
            if (arquivo_buscado != NULL) {
                return -2;    // já existe arquivo com esse nome
            }

            int endereco_disco = first_fit_disco(tamanho_bloco, disco);

            if (endereco_disco > -1) {
                int index = disco->diretorio.total_arquivos;
                if (index < MAX_ARQUIVOS) {
                    Arquivo novo;
                    novo.nome = nome_arquivo;
                    novo.pid_dono = processo->pid;
                    novo.bloco_inicial = endereco_disco;
                    novo.tamanho = tamanho_bloco;
                    novo.slot_valido = 1;

                    adicionar_no_diretorio(&novo, disco);

                    // Preenche os blocos no disco
                    for (int i = 0; i < tamanho_bloco; i++) {
                        disco->blocos[endereco_disco + i] = nome_arquivo;
                    }
                    return 1; 
                } else {
                    return -6; // Diretório sem espaço
                }
            } else {
                return -1;
            }

        case 1: // Operação de DELETAR arquivo
            if (arquivo_buscado == NULL) {
                return -4;   // não foi encontrado o arquivo a ser deletado
            }

            // Verifica permissão (tempo real ou o próprio dono)
            if (processo->prioridade == 0 || processo->pid == arquivo_buscado->pid_dono) {
                int endereco_inicial = arquivo_buscado->bloco_inicial;
                int tamanho = arquivo_buscado->tamanho;

                // Libera os blocos no disco
                for (int i = 0; i < tamanho; i++) {
                    disco->blocos[endereco_inicial + i] = '0'; // Marca como livre
                }

                remover_do_diretorio(index_arquivo, disco);
                return 2;
            } else {
                return -3;   // Esse processo não pode deletar esse arquivo
            }

        default:
            // Erro: Operação desconhecida
            return -5;
    }
    return 0; 
}