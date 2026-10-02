#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"

// Todas retornam 1 em sucesso e 0 se fopen/IO falhar.

// Texto 
int arquivo_salvar_alunos_txt(const char *arq, const Aluno v[], int qtd);
int arquivo_carregar_alunos_txt(const char *arq, Aluno v[], int max, int *qtd);
int arquivo_salvar_disciplinas_txt(const char *arq, const Disciplina v[], int qtd);
int arquivo_carregar_disciplinas_txt(const char *arq, Disciplina v[], int max, int *qtd);

// Binario: carga e gravacao completas 
int arquivo_salvar_matriculas_bin(const char *arq, const Matricula v[], int qtd);
int arquivo_carregar_matriculas_bin(const char *arq, Matricula v[], int max, int *qtd);

// Binario: acesso direto com fseek 
int arquivo_ler_matricula_bin(const char *arq, int pos, Matricula *m);
int arquivo_atualizar_matricula_bin(const char *arq, int pos, const Matricula *m);
int arquivo_contar_matriculas_bin(const char *arq);   

#endif