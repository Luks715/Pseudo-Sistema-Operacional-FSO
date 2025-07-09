#ifndef ARQUIVO_H
#define ARQUIVO_H

typedef struct {
    char nome;
    int pid_dono;
    int bloco_inicial;
    int tamanho;
    int slot_valido; // utilizado para verificar se a posição na struct diretório está ocupada ou não
} Arquivo;

#endif
