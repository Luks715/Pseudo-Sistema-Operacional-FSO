#include <stdio.h>
#include <stdlib.h>

#include "disco.h"
#include "arquivo.h"
#include "processo.h"

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

int criar_arquivo(int endereco_disco, char nome_arquivo, int tamanho_bloco, Disco* disco){

}

Arquivo* buscar_arquivo(char nome_arquivo, Disco* disco){
    for(int i = 0; i < disco->diretorio.total_arquivos; i++){
        if(disco->diretorio.arquivos[i].nome == nome_arquivo){
            // retorna o pID do arquivo, se for encontrado
            return &(disco->diretorio.arquivos[i]);
        }
    }

    // arquivo não encontrado
    return NULL;
}

void deletar_arquivo(const char* nome, Disco* disco){
    for(int i = 0; i < disco->total_blocos)

}

int sistema_arquivos(int ID_processo, int operacao, char nome_arquivo, int tamanho_bloco, Disco* disco){
    // Busca se já existe um arquivo com esse nome, -1: não existe, >=0: existe
    int pID_arquivo = buscar_arquivo(nome_arquivo, disco);

    switch(operacao){
        case 0:
            // Se não existe um arquivo com esse nome, dá segmento ao processo de criação
            if(pID_arquivo == -1){
                // Procura no disco se há espaço disponível
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
            // Verifica se o processo pode deletar esse arquivo (tempo real ou o criador do arquivo)
            if(pID_arquivo == ID_processo || buscar_processo(ID_processo).prioridade == 0){
                deletar_arquivo(nome_arquivo, disco);
                return 1;
            } else {
                // Esse processo não pode deletar esse arquivo
                return -3;
            }
            break;

        default:

    }   
}