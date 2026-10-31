#ifndef ALUNO_H
#define ALUNO_H

#define TAM_NOME   50
#define TAM_CURSO  30
#define RA_MIN     1
#define RA_MAX     99999

#define ALUNO_OK              0
#define ALUNO_ERRO_REPETIDO   1
#define ALUNO_ERRO_MEMORIA    2
#define ALUNO_ERRO_NAO_ACHOU  -1

typedef struct {
    int  ra;
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
} Aluno;

typedef struct {
    Aluno *dados;
    int    qtd;
    int    capacidade;
} ListaAlunos;

void  aluno_lista_iniciar(ListaAlunos *l);
void  aluno_lista_liberar(ListaAlunos *l);

Aluno aluno_ler(void);
int   aluno_cadastrar(ListaAlunos *l, Aluno novo);
int   aluno_buscar(const ListaAlunos *l, int ra);
int   aluno_remover(ListaAlunos *l, int ra);
void  aluno_listar(const ListaAlunos *l);

#endif