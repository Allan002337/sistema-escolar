#ifndef BOLETIM_H
#define BOLETIM_H

#include <stdio.h>
#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"

#define NOTA_APROVACAO   6.0f   // media >= 6 aprova 
#define NOTA_RECUPERACAO 5.0f   // media >= 5 e < 6 recuperacao; < 5 reprova 
#define MIN_NOTAS_FINAL   2     // com menos notas que isso, a situacao é parcial

#define BOLETIM_OK 0
#define BOLETIM_ERRO_PARAMETRO 1


typedef enum { SIT_APROVADO, SIT_RECUPERACAO, SIT_REPROVADO, SIT_PARCIAL, SIT_SEM_NOTAS} Situacao;

// RECURSIVA: soma v[0..n-1]. Caso base: n <= 0 
float boletim_somar_recursivo(const float v[], int n);

float       boletim_media(const float v[], int n);   // usa a soma recursiva; 0 se n <= 0 
Situacao    boletim_situacao(float media, int qtdNotasLancadas);  // considera apenas notas lancadas (qtdNotasLancadas)
const char *boletim_texto_situacao(Situacao s); // devolve "Aprovado", "Recuperacao", "Reprovado" ou "Parcial" ou "-" =  sem notas lancadas. conforme a situacao s

// Emite boletim do aluno para o arquivo de saída, considerando as disciplinas e matrículas fornecidas, devolvendo BOLETIM_OK ou BOLETIM_ERRO_PARAMETRO se algum parâmetro for inválido.
int boletim_emitir(FILE *saida, const Aluno *aluno,
                    const ListaDisciplinas *disciplinas,
                    const ListaMatriculas *matriculas,
                    int semestre);

#endif