#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"

/*  0 = sucesso, positivo = erro.
   Excecao: arquivo_contar_matriculas_bin devolve a quantidade (>= 0) ou -1. */
#define ARQUIVO_OK              0
#define ARQUIVO_NAO_EXISTE      1   /* carregar: arquivo ainda nao existe (1a execucao). Nao e falha grave: a lista fica vazia */
#define ARQUIVO_ERRO_ABRIR      2   /* fopen falhou por outro motivo (pasta inexistente, sem permissao...) */
#define ARQUIVO_ERRO_IO         3   /* fread/fwrite/fseek/fclose falhou */
#define ARQUIVO_ERRO_FORMATO    4   /* linha mal formatada, RA repetido no arquivo ou binario truncado */
#define ARQUIVO_ERRO_MEMORIA    5   /* malloc/realloc falhou */
#define ARQUIVO_ERRO_PARAMETRO  6   /* ponteiro NULL, ou lista que deveria estar vazia nao estava */
#define ARQUIVO_ERRO_POSICAO    7   /* pos fora de 0..qtd-1 */


/* alunos.txt:      ra;nome;curso
   disciplinas.txt: codigo;nome;creditos           */
int arquivo_salvar_alunos_txt(const char *arq, const ListaAlunos *l);
int arquivo_carregar_alunos_txt(const char *arq, ListaAlunos *l);           /* a lista deve estar vazia */
int arquivo_salvar_disciplinas_txt(const char *arq, const ListaDisciplinas *l);
int arquivo_carregar_disciplinas_txt(const char *arq, ListaDisciplinas *l); /* a lista deve estar vazia */

/* Binario: gravacao e carga completas */
int arquivo_salvar_matriculas_bin(const char *arq, const ListaMatriculas *l);
int arquivo_carregar_matriculas_bin(const char *arq, ListaMatriculas *l);   /* a lista deve estar vazia */

/* Binario: acesso direto com fseek (pos comeca em 0) */
int arquivo_ler_matricula_bin(const char *arq, int pos, Matricula *m);
int arquivo_atualizar_matricula_bin(const char *arq, int pos, const Matricula *m);
int arquivo_contar_matriculas_bin(const char *arq);   /* quantidade de registros, ou -1 em falha */

/* Boletim em arquivo texto  */
/* Abre o arquivo, chama boletim_emitir e fecha. */
int arquivo_exportar_boletim_txt(const char *arq, const Aluno *aluno,
                                 const ListaDisciplinas *disciplinas,
                                 const ListaMatriculas *matriculas, int semestre);

#endif