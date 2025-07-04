#include <stdio.h>
#include <stdlib.h>

#include "arquivo.h"
#include "sistema_arquivos.h"

int first_fit(int tamanho, Disco* disco){
    // Variáveis para armazenar a quantidade de blocos livres e o endereço inicial
    int blocos_livres = 0;
    int endereco_inicial;

    // Itera por cada bloco do disco, contabilizando a sequência de blocos livres
    for(int i = 0; i < disco->total_blocos; i++){
        if(disco->blocos[i] == 0){
            if(blocos_livres == 0){
                endereco_inicial = i;
            }

            blocos_livres++;
        } else {
            blocos_livres = 0;
        }

        if(blocos_livres == tamanho){
            return endereco_inicial;
        }
    }

    // Se o iterador chegar ao fim do loop e não encontrar uma sequência grande o suficiente para armazenar o arquivo, retorna erro
    return -1;
}

void criar_arquivo(int endereco_inicial, const char* nome_arquivo, int tamanho, Disco* disco){
    for(int i = 0; i < tamanho; i++){
        disco->blocos[endereco_inicial + i] = nome_arquivo;
    }
}

Arquivo* buscar_arquivo(const char* nome_arquivo, Disco* disco){
    for(int i = 0; i < disco->diretorio.total_arquivos; i++){
        if(strcmp(disco->diretorio.arquivos[i].nome, nome_arquivo) == 0){
            // retorna o pID do arquivo, se for encontrado
            return &(disco->diretorio.arquivos[i]);
        }
    }

    // arquivo não encontrado
    return NULL;
}

void deletar_arquivo(Arquivo* arquivo, Disco* disco){
    int endereco_inicial = arquivo->bloco_inicial;
    int tamanho = arquivo->tamanho;

    for(int i = 0; i < tamanho; i++){
        disco->blocos[endereco_inicial + i] = 0;
    }
}

int sistema_arquivos(int operacao, const char* nome_arquivo, int tamanho_bloco, Disco* disco, Processo* processo){
    // Busca se já existe um arquivo com esse nome, retorna um ponteiro para o arquivo se sim, NULL para não
    Arquivo* arquivo_buscado = buscar_arquivo(nome_arquivo, disco);

    switch(operacao){
        case 0:
            // Se não existe um arquivo com esse nome, dá contiuação ao processo de criação
            if(arquivo_buscado == NULL){
                // Procura no disco se há espaço disponível para criar o arquivo
                int endereco_disco = first_fit(tamanho_bloco, disco);

                if(endereco_disco > -1){
                    criar_arquivo(endereco_disco, nome_arquivo, tamanho_bloco, disco);
                    return 1;
                } else {
                    // Não há espaço no disco para criar o arquivo com o tamanho determinado
                    return -1;
                }
            } else {
                // Não foi possível criar o arquivo pois já existe um arquivo com esse nome
                return -2;
            }

            break;
        case 1:
            // Se o arquivo existe, dá contiação ao processo de destruição
            if(arquivo_buscado != NULL){
                // Verifica se o processo pode deletar esse arquivo (tempo real ou o criador do arquivo)
                if(processo->prioridade == 0 || processo->pid == arquivo_buscado->pid_dono){
                    deletar_arquivo(arquivo_buscado, disco);
                    return 2;
                } else {
                    // Esse processo não pode deletar esse arquivo
                    return -3;
                }
            } else {
                // Nenhum arquivo com o nome desejado foi encontrado
                return -4;
            }

            break;
        default:
            // Operação não suportada pelo Sistema de Arquivos
            return -5;
            break;
    }   
}