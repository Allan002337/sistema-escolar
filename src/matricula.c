#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matricula.h"
#include "aluno.h"
#include "disciplina.h"

void matricula_lista_iniciar(ListaMatriculas *l)
{
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}

void matricula_lista_liberar(ListaMatriculas *l)
{
    free(l->dados);
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}


int  matricula_efetuar(ListaMatriculas *l, const Aluno *aluno, const Disciplina *disciplina, int semestre)
{
    if (aluno == NULL || disciplina == NULL)
        return MATRICULA_ERRO_PARAMETRO; // Parâmetro inválido

    if (semestre < SEMESTRE_MIN || semestre > SEMESTRE_MAX  || semestre % 10 < 1 || semestre % 10 > 2)
        return MATRICULA_ERRO_SEMESTRE; // Semestre fora do intervalo permitido

    if (matricula_buscar(l, aluno->ra, disciplina->codigo, semestre) != MATRICULA_NAO_ENCONTRADA)
        return MATRICULA_ERRO_REPETIDA; // O aluno já está matriculado na disciplina neste semestre

    if (l->qtd >= l->capacidade)
    {
        int nova_capacidade = (l->capacidade == 0) ? 1 : l->capacidade * 2;
        Matricula *novo_dados = realloc(l->dados, nova_capacidade * sizeof(Matricula));
        if (novo_dados == NULL)
            return MATRICULA_ERRO_MEMORIA; // Falha na alocação de memória

        l->dados = novo_dados;
        l->capacidade = nova_capacidade;
    }

    l->dados[l->qtd].raAluno = aluno->ra;
    l->dados[l->qtd].codDisciplina = disciplina->codigo;    
    l->dados[l->qtd].semestre = semestre;

    l->dados[l->qtd].qtdNotas = 0; // Inicializa a quantidade de notas como 0
    for (int j = 0; j < MAX_AVALIACOES; j++)
    {
        l->dados[l->qtd].notas[j] = 0.0f; // Inicializa cada nota como 0.0
    }

    l->qtd++;

    return MATRICULA_OK; // Matrícula efetuada com sucesso
}

int  matricula_buscar(const ListaMatriculas *l, int raAluno, int codDisciplina, int semestre)
{
    for (int i = 0; i < l->qtd; i++)
    {
        if (l->dados[i].raAluno == raAluno && l->dados[i].codDisciplina == codDisciplina && l->dados[i].semestre == semestre)
        {
            return i; // Retorna o índice da matrícula encontrada
        }
    }
    return MATRICULA_NAO_ENCONTRADA; // Retorna -1 se a matrícula não for encontrada
}

int  matricula_lancar_nota(ListaMatriculas *l, int idx, float nota)
{
    if (idx < 0 || idx >= l->qtd)
        return MATRICULA_ERRO_INDICE_INVALIDA; // Índice inválido

    if (nota < NOTA_MIN || nota > NOTA_MAX)
        return MATRICULA_ERRO_NOTA_INVALIDA; // Nota fora do intervalo permitido

    if (l->dados[idx].qtdNotas >= MAX_AVALIACOES)
        return MATRICULA_ERRO_NOTAS_CHEIAS; // Não é possível adicionar mais notas

    l->dados[idx].notas[l->dados[idx].qtdNotas] = nota;
    l->dados[idx].qtdNotas++;

    return MATRICULA_OK; // Nota lançada com sucesso
}

void matricula_listar_aluno(const ListaMatriculas *l, int raAluno)
{
    int cont = 0;

    for (int i = 0; i < l->qtd; i++)
    {
        if (l->dados[i].raAluno == raAluno)
        {
            cont++;
            printf("Disciplina: %d, Semestre: %d, Notas: ", l->dados[i].codDisciplina, l->dados[i].semestre);

            if (l->dados[i].qtdNotas == 0)
                printf("Sem notas lancadas");
            else
                for (int j = 0; j < l->dados[i].qtdNotas; j++)
                {
                    printf("%.2f ", l->dados[i].notas[j]);
                }
            printf("\n");
        }
    }

    if (cont == 0)
        {
            printf("Nenhuma matricula encontrada para o aluno RA: %d\n", raAluno);
        }
}

void matricula_listar_disciplina(const ListaMatriculas *l, int codDisciplina, int semestre)
{
    int cont = 0;

    for (int i = 0; i < l->qtd; i++)
    {
        if (l->dados[i].codDisciplina == codDisciplina && l->dados[i].semestre == semestre)
        {
            cont++;
            printf("Aluno: %d, Notas: ", l->dados[i].raAluno);

            if (l->dados[i].qtdNotas == 0)
                printf("Sem notas lancadas");
            else
            {
                for (int j = 0; j < l->dados[i].qtdNotas; j++)
                {
                    printf("%.2f ", l->dados[i].notas[j]);
                }
            }
            printf("\n");
        }
    }

    if (cont == 0)
    {
        printf("Nenhuma matricula encontrada para a disciplina: %d no semestre: %d\n", codDisciplina, semestre);
    }
}

int matricula_remover(ListaMatriculas *l, int raAluno, int codDisciplina, int semestre)
{
    int idx = matricula_buscar(l, raAluno, codDisciplina, semestre);

    if (idx == MATRICULA_NAO_ENCONTRADA)
        return MATRICULA_ERRO_NAO_ACHOU; // Matrícula não encontrada

    for (int i = idx; i < l->qtd - 1; i++)
    {
        l->dados[i] = l->dados[i + 1];
    }
    l->qtd--;

    return MATRICULA_OK; // Matrícula removida com sucesso
}

int matricula_remover_por_aluno(ListaMatriculas *l, int raAluno, int *removidas)
{
    if (removidas == NULL)
        return MATRICULA_ERRO_PARAMETRO; // Parâmetro inválido

    *removidas = 0;

    int esc = 0;
    int qtdAntiga = l->qtd; 

    for (int le = 0; le < qtdAntiga; le++)
    {
        if (l->dados[le].raAluno != raAluno)
        {
            l->dados[esc] = l->dados[le];
            esc++;
        }
    }

    l->qtd = esc;

    *removidas = qtdAntiga - esc;

    return MATRICULA_OK;
}
