#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "../include/arquivo.h"
#include "sistema_arquivos.h"

int first_fit(int tamanho, Disco* disco) {
    int blocos_livres = 0;
    int endereco_inicial = -1;

    for (int i = 0; i < disco->total_blocos; i++) {
        if (disco->blocos[i] == 0) {
            if (blocos_livres == 0) {
                endereco_inicial = i;
            }
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

Arquivo* buscar_arquivo(const char nome_arquivo, Disco* disco) {
    for (int i = 0; i < disco->diretorio.total_arquivos; i++) {
        if (disco->diretorio.arquivos[i].nome == nome_arquivo) {
            return &(disco->diretorio.arquivos[i]);
        }
    }
    return NULL; // Arquivo não encontrado
}


void remover_do_diretorio(Arquivo* arquivo, Disco* disco) {
    int encontrado = 0;
    for (int i = 0; i < disco->diretorio.total_arquivos; i++) {
        // Encontra o arquivo a ser removido
        if (&(disco->diretorio.arquivos[i]) == arquivo) {
            encontrado = 1;
        }
        // Se o arquivo já foi encontrado, desloca os próximos elementos para a esquerda
        if (encontrado && i < disco->diretorio.total_arquivos - 1) {
            disco->diretorio.arquivos[i] = disco->diretorio.arquivos[i + 1];
        }
    }

    if (encontrado) {
        disco->diretorio.total_arquivos--;
    }
}


int sistema_arquivos(int operacao, const char* nome_arquivo, int tamanho_bloco, Disco* disco, Processo* processo) {
    Arquivo* arquivo_buscado = buscar_arquivo(*nome_arquivo, disco);

    switch (operacao) {
        case 0: // Operação de CRIAR arquivo
            if (arquivo_buscado != NULL) {
                return -2;
            }

            int endereco_disco = first_fit(tamanho_bloco, disco);

            if (endereco_disco > -1) {
                int index = disco->diretorio.total_arquivos;
                if (index < MAX_ARQUIVOS) {
                    disco->diretorio.arquivos[index].nome = *nome_arquivo;
                    disco->diretorio.arquivos[index].pid_dono = processo->pid;
                    disco->diretorio.arquivos[index].bloco_inicial = endereco_disco;
                    disco->diretorio.arquivos[index].tamanho = tamanho_bloco;
                    disco->diretorio.total_arquivos++;

                    // Preenche os blocos no disco
                    for (int i = 0; i < tamanho_bloco; i++) {
                        disco->blocos[endereco_disco + i] = *nome_arquivo;
                    }
                    return 1; 
                } else {
                    return -6; 
                }
            } else {
                return -1;
            }

        case 1: // Operação de DELETAR arquivo
            if (arquivo_buscado == NULL) {
                return -4;
            }

            // Verifica permissão (tempo real ou o próprio dono)
            if (processo->prioridade == 0 || processo->pid == arquivo_buscado->pid_dono) {
                int endereco_inicial = arquivo_buscado->bloco_inicial;
                int tamanho = arquivo_buscado->tamanho;

                // Libera os blocos no disco
                for (int i = 0; i < tamanho; i++) {
                    disco->blocos[endereco_inicial + i] = 0; // Marca como livre
                }

                remover_do_diretorio(arquivo_buscado, disco);
                return 2;
            } else {
                return -3;
            }

        default:
            // Erro: Operação desconhecida
            return -5;
    }
    return 0; 
}