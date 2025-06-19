#ifndef BLOCO_H
#define BLOCO_H

// Essa struct Bloco é para ser usada somente na Memória, não no disco
// Já que o bitmap do Disco não precisa ter o pid, apenas '0' para livre e 
// O nome do arquivo para simbolizar "ocupado", portanto, é um vetor de char

typedef struct {
    int ocupado; // 0 = livre, 1 = ocupado
    int pid;  // processo dono
} Bloco;

#endif
