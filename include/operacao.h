#ifndef OPERACAO_H
#define OPERACAO_H

typedef struct {
    int pid;             // Processo dono da operação
    int codigo_op;       // 0 = criar, 1 = deletar
    char nome_arquivo;  
    int blocos;          // Apenas se for operação de criação (0 se deletar)
} Operacao;

#endif
