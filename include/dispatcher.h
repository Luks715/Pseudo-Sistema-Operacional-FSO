#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "processo.h"

void dispatcher(const char* arq_proc, const char* arq_files);
void executar_operacoes_de_arquivo(Processo* processo_atual);

#endif // DISPATCHER_H