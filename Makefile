# ==========================================================================
# Makefile - Sistema Escolar
#
# Uso (na raiz do projeto):
#   make          compila e gera o executavel "sistema"
#   make run      compila (se preciso) e executa
#   make debug    recompila com verificador de memoria (Linux/WSL/MinGW com ASan)
#   make clean    apaga os arquivos gerados pela compilacao
#
# Obs.: o Makefile usa mkdir -p e rm -rf. No Windows funciona no Git Bash
# ou no MSYS2. No PowerShell/CMD puro, use o comando gcc do README.
# ==========================================================================

CC      = gcc
# -Wall -Wextra : mostra avisos (tratem todos como erro)
# -std=c99      : padrao C99
# -Iinclude     : permite #include "aluno.h" sem escrever o caminho
# -MMD -MP      : gera arquivos .d com as dependencias de cada .c; assim, se um
#                 .h mudar (ex.: MAX_AVALIACOES), todos os .c que o incluem
#                 sao recompilados automaticamente
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude -MMD -MP
LDFLAGS =

TARGET  = sistema
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:src/%.c=build/%.o)
DEP     = $(OBJ:.o=.d)

all: $(TARGET)

# Ligacao: junta todos os .o no executavel
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compilacao: cada src/x.c vira build/x.o
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Executa o programa (cria a pasta data/ se nao existir)
run: $(TARGET)
	@mkdir -p data
	./$(TARGET)

# Versao de depuracao: o AddressSanitizer acusa acesso fora de vetor e
# vazamento de memoria. Faz clean antes para nao misturar objetos.
debug: CFLAGS  += -g -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	rm -rf build $(TARGET) $(TARGET).exe

-include $(DEP)

.PHONY: all run debug clean