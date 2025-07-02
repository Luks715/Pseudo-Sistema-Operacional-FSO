#include <stdio.h>
#include <stdlib.h>

#include "processo.h"
#include "memoria.h"

#include "alocador.h"

// Função responsável por alocar memória para um processo
int alocador(Processo* p, Memoria* m){
    int bloco_inicial;
    int bloco_final;
    int tamanho_processo = p->blocos_mem;

    // Com base na prioridade do Processo, define o range de onde ele pode ser armazenado na memória
    if(p->prioridade == 0){ 
        bloco_inicial = 0;
        bloco_final = 63;
    } else {
        bloco_inicial = 64;
        bloco_final = 1023;
    }

    // Se há espaço para alocar o processo, retorna o endereço inicial do bloco, se não, retorna -1
    int resultado_alocacao = first_fit(bloco_inicial, bloco_final, tamanho_processo, m);

    if(resultado_alocacao != -1) {
        // Altera os blocos na memória onde o processo está alocado para 1
        for(int i = 0; i <= tamanho_processo; i++){
            m->blocos[resultado_alocacao + i].ocupado = 1;
            m->blocos[resultado_alocacao + i].pid = p->pid;
        }

        p->offset_mem = resultado_alocacao;
    } 

    return resultado_alocacao;
}

// Itera pelos blocos de memória procurando a primeira sequência de blocos igual a quantidade exigida pelo processo
int first_fit(int bloco_inicial, int bloco_final, int tamanho_processo, Memoria* m){
    int blocos_livres = 0;  // sequência de blocos que irá armazenar o processo
    int inicio_segmento;

    for(int i = bloco_inicial; i < bloco_final; i++){
        if(m->blocos[i].ocupado == 0) {
            if (blocos_livres == 0) {
                inicio_segmento = i;  // armazena o bloco inicial da sequência
            }

            blocos_livres++;
        } else {
            blocos_livres = 0;
        }

        if(blocos_livres == tamanho_processo){
            return inicio_segmento;
        }
    }

    // Se todo o loop for percorrido sem achar uma sequência de blocos, significa que não existe 
    // sequência de blocos livres grande o suficiente para abrigar todo o processo. Fragmentação
    return -1;
}