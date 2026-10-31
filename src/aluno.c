#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"
#include "utils.h"

void aluno_lista_iniciar(ListaAlunos *l)
{
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}

void aluno_lista_liberar(ListaAlunos *l)
{
    free(l->dados);
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}

Aluno aluno_ler(void)
{
    Aluno novo;
    novo.ra = lerInteiro("Digite o RA do aluno: ", RA_MIN, RA_MAX); // Limita o RA entre 1 e 99999

    do 
    {
        lerString("Digite o nome do aluno: ", novo.nome, TAM_NOME); // Usa a função lerString para ler o nome
    }
    while (strlen(novo.nome) == 0);

    lerString("Digite o curso do aluno: ", novo.curso, TAM_CURSO); // Usa a função lerString para ler o curso

    return novo;
}

int   aluno_cadastrar(ListaAlunos *l, Aluno novo)
{
    // Verifica se o RA já existe
    if(aluno_buscar(l, novo.ra) != ALUNO_ERRO_NAO_ACHOU)
    {
        return ALUNO_ERRO_REPETIDO; // RA repetido
    }

    if(l->qtd >= l->capacidade)
    {
        int nova_capacidade = (l->capacidade == 0) ? 1 : l->capacidade * 2;
        Aluno *novo_dados = realloc(l->dados, nova_capacidade * sizeof(Aluno));
        if (novo_dados == NULL)
        {
            return ALUNO_ERRO_MEMORIA; // falha na alocação
        }
        l->dados = novo_dados;
        l->capacidade = nova_capacidade;
    }

    l->dados[l->qtd] = novo;
    l->qtd++;
    return 0; // sucesso
}

int   aluno_buscar(const ListaAlunos *l, int ra)
{
    for (int i = 0; i < l->qtd; i++)
    {
        if (l->dados[i].ra == ra)
            return i;
    }
    return ALUNO_ERRO_NAO_ACHOU; // não encontrado
}

int   aluno_remover(ListaAlunos *l, int ra)
{
    int index = aluno_buscar(l, ra);
    if (index == ALUNO_ERRO_NAO_ACHOU)
    {
        return ALUNO_ERRO_NAO_ACHOU; // não encontrou
    }

    for (int i = index; i < l->qtd - 1; i++)
    {
        l->dados[i] = l->dados[i + 1];
    }
    l->qtd--;

    return ALUNO_OK; 
}      
void  aluno_listar(const ListaAlunos *l)
{
    if (l->qtd == 0)
    {
        printf("Nenhum aluno cadastrado.\n");
        return;
    }

    printf("Lista de Alunos:\n");
    for (int i = 0; i < l->qtd; i++)
    {
        printf("RA: %d, Nome: %s, Curso: %s\n", l->dados[i].ra, l->dados[i].nome, l->dados[i].curso);
    }
}