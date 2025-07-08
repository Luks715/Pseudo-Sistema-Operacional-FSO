#ifndef SISTEMA_ARQUIVOS_H
#define SISTEMA_ARQUIVOS_H

#include "../include/disco.h"
#include "../include/processo.h"

int sistema_arquivos(int operacao, const char* nome_arquivo, int tamanho_bloco, Disco* disco, Processo* processo);

#endif
