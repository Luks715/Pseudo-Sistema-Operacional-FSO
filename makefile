# Nome do executável final
EXEC = pseudoSO

# Diretórios
SRC_DIR = .
INCLUDE_DIR = include
MEM_DIR = gerenciador_de_memoria
ARQ_DIR = gerenciador_de_arquivos
PROC_DIR = gerenciador_de_processos
ES_DIR = gerenciador_de_ES
UTILS_DIR = utils

# Lista de arquivos fonte
SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/dispatcher.c \
      $(UTILS_DIR)/semaforo.c \
      $(UTILS_DIR)/fila.c \
      $(MEM_DIR)/alocador.c \
      $(ARQ_DIR)/sistema_arquivos.c \
      $(PROC_DIR)/gerenciador_processos.c \
      $(PROC_DIR)/escalonador.c \
      $(ES_DIR)/gerenciador_es.c

# Flags do compilador
CFLAGS = -Wall -Wextra -std=c99 -I$(INCLUDE_DIR)

# Comando padrão
all: $(EXEC)

$(EXEC): $(SRC)
	@gcc $(CFLAGS) $(SRC) -o $(EXEC)
	@echo "Compilação concluída! Executável: ./$(EXEC)"

# Limpar arquivos gerados
clean:
	@rm -f $(EXEC)
	@echo "Executável removido."

