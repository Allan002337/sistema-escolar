#ifndef BOLETIM_H
#define BOLETIM_H

#include <stdio.h>
#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"

#define NOTA_APROVACAO   6.0f   // media >= 6 aprova 
#define NOTA_RECUPERACAO 5.0f   // media >= 5 e < 6 recuperacao; < 5 reprova 

typedef enum { APROVADO, RECUPERACAO, REPROVADO } Situacao;

// RECURSIVA: soma v[0..n-1]. Caso base: n == 0 
float boletim_somar_recursivo(const float v[], int n);

float       boletim_media(const float v[], int n);   // usa a soma recursiva; 0 se n <= 0 
Situacao    boletim_situacao(float media);
const char *boletim_texto_situacao(Situacao s);

/* Escreve em qualquer FILE*: stdout (tela) ou arquivo texto.
   Considera apenas matriculas do aluno com notasLancadas == 1. */
void boletim_emitir(FILE *saida, const Aluno *aluno,
                    const Disciplina disc[], int qtdDisc,
                    const Matricula matriculas[], int qtdMatriculas);

#endif