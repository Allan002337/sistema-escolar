#ifndef DISCIPLINA_H
#define DISCIPLINA_H

#define MAX_DISC      30
#define TAM_NOME_DISC 40

typedef struct {
    int  codigo;
    char nome[TAM_NOME_DISC];
    int  creditos;
    int  cargaHoraria;
} Disciplina;

Disciplina disciplina_ler(void);
int  disciplina_cadastrar(Disciplina disc[], int *qtd, Disciplina nova); // 0 se cheio ou codigo repetido 
int  disciplina_buscar(const Disciplina disc[], int qtd, int codigo);    // indice ou -1 
void disciplina_listar(const Disciplina disc[], int qtd);

#endif