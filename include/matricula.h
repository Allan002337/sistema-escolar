#ifndef MATRICULA_H
#define MATRICULA_H

#define MAX_MATRICULAS 500
#define NUM_AVALIACOES 6  //definir o valor final do grupo 

typedef struct {
    int   raAluno;
    int   codDisciplina;
    int   semestre;                  
    float notas[NUM_AVALIACOES];
    int   notasLancadas;             // 0 = so matriculado, 1 = notas lancadas 
} Matricula;

// 0 se cheio ou se o aluno ja estiver na disciplina neste semestre 
int  matricula_efetuar(Matricula m[], int *qtd, int raAluno, int codDisciplina, int semestre);
// indice ou -1 
int  matricula_buscar(const Matricula m[], int qtd, int raAluno, int codDisciplina, int semestre);
// pede as notas (0 a 10) da matricula m[idx]; 0 se idx invalido 
int  matricula_lancar_notas(Matricula m[], int qtd, int idx);
void matricula_listar_aluno(const Matricula m[], int qtd, int raAluno);

#endif