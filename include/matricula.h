#ifndef MATRICULA_H
#define MATRICULA_H

#include "aluno.h"
#include "disciplina.h"

#define MAX_AVALIACOES 4
#define MATRICULA_OK 0
#define MATRICULA_ERRO_REPETIDA 1
#define MATRICULA_ERRO_MEMORIA 2
#define MATRICULA_NAO_ENCONTRADA -1
#define MATRICULA_ERRO_INDICE_INVALIDA 3
#define MATRICULA_ERRO_NOTA_INVALIDA 4
#define MATRICULA_ERRO_NOTAS_CHEIAS 5
#define MATRICULA_ERRO_PARAMETRO  6   /* aluno ou disciplina NULL */
#define MATRICULA_ERRO_SEMESTRE   7   /* fora de SEMESTRE_MIN..SEMESTRE_MAX */
#define MATRICULA_ERRO_NAO_ACHOU 8   

#define SEMESTRE_MIN 20001
#define SEMESTRE_MAX 99992

#define NOTA_MIN 0.0f
#define NOTA_MAX 10.0f

typedef struct {
    int   raAluno;
    int   codDisciplina;
    int   semestre;                  
    float notas[MAX_AVALIACOES];
    int   qtdNotas;             // 0 = so matriculado, 1 ou mais = notas lancadas 
} Matricula;

typedef struct {
    Matricula *dados;
    int        qtd;
    int        capacidade;
} ListaMatriculas;

void matricula_lista_iniciar(ListaMatriculas *l); //inicializa a lista de matriculas, deixando vazia e com capacidade 0
void matricula_lista_liberar(ListaMatriculas *l); //libera a memória alocada para a lista de matriculas, se houver

// MATRICULA_ERRO_REPETIDA se o aluno ja estiver na disciplina neste semestre 
int  matricula_efetuar(ListaMatriculas *l, const Aluno *aluno, const Disciplina *disciplina, int semestre);

int  matricula_buscar(const ListaMatriculas *l, int raAluno, int codDisciplina, int semestre); // devolve o índice da matricula ou MATRICULA_NAO_ENCONTRADA == -1 
int  matricula_lancar_nota(ListaMatriculas *l, int idx, float nota); // grava a nota (0 a 10) da matricula m[idx]; MATRICULA_ERRO_INDICE_INVALIDA se idx invalido, MATRICULA_ERRO_NOTA_INVALIDA se nota invalida, MATRICULA_ERRO_NOTAS_CHEIAS se ja tiver MAX_AVALIACOES notas
void matricula_listar_aluno(const ListaMatriculas *l, int raAluno); // lista todas as disciplinas do aluno raAluno, com notas 
void matricula_listar_disciplina(const ListaMatriculas *l, int codDisciplina, int semestre); // lista todos os alunos matriculados na disciplina codDisciplina no semestre, com notas 
int matricula_remover(ListaMatriculas *l, int raAluno, int codDisciplina, int semestre); // remove a matricula do aluno raAluno na disciplina codDisciplina no semestre; devolve MATRICULA_OK == 0 ou MATRICULA_ERRO_NAO_ACHOU == 8
int matricula_remover_por_aluno(ListaMatriculas *l, int raAluno, int *removidas); // remove todas as matriculas do aluno raAluno; 


#endif