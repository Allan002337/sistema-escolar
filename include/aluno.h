#ifndef ALUNO_H
#define ALUNO_H

#define MAX_ALUNOS 100
#define TAM_NOME   50
#define TAM_CURSO  30

typedef struct {
    int  ra;
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
} Aluno;

Aluno aluno_ler(void);                                       //pede os dados ao usuario 
int   aluno_cadastrar(Aluno alunos[], int *qtd, Aluno novo); // 0 se cheio ou RA repetido 
int   aluno_buscar(const Aluno alunos[], int qtd, int ra);   // indice ou -1 
int   aluno_remover(Aluno alunos[], int *qtd, int ra);       // 0 se nao encontrou 
void  aluno_listar(const Aluno alunos[], int qtd);

#endif