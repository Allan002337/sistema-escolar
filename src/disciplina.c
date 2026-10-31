#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disciplina.h"
#include "utils.h"

void disciplina_lista_iniciar(ListaDisciplinas *l)
{
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}

void disciplina_lista_liberar(ListaDisciplinas *l)
{
    free(l->dados);
    l->dados = NULL;
    l->qtd = 0;
    l->capacidade = 0;
}

Disciplina disciplina_ler(const ListaDisciplinas *l)
{
    Disciplina disc;

    do
    {
        disc.codigo = lerInteiro("Informe o codigo da disciplina: ", 1, 9999);

        if (disciplina_buscar(l, disc.codigo) != DISCIPLINA_NAO_ENCONTRADA) // Verifica se o código já existe
            printf("Codigo ja cadastrado. Tente novamente.\n");
        else 
            break; // Código único, sai do loop
        
    } while (1); 

    do
    {
        lerString("Informe o nome da disciplina: ", disc.nome, TAM_NOME_DISC);
    }
    while (strlen(disc.nome) == 0);

    disc.creditos = lerInteiro("Informe os creditos da disciplina: ", 1, 10);

    return disc;
}

int  disciplina_cadastrar(ListaDisciplinas *l, Disciplina nova)
{
    if (disciplina_buscar(l, nova.codigo) != DISCIPLINA_NAO_ENCONTRADA)
    {
        return DISCIPLINA_ERRO_REPETIDA; // Código repetido
    }

    if (l->qtd >= l->capacidade)
    {
        int nova_capacidade = (l->capacidade == 0) ? 1 : l->capacidade * 2;
        Disciplina *novo_dados = realloc(l->dados, nova_capacidade * sizeof(Disciplina));
        if (novo_dados == NULL)
        {
            return DISCIPLINA_ERRO_MEMORIA; // Falha na alocação
        }
        l->dados = novo_dados;
        l->capacidade = nova_capacidade;
    }

    l->dados[l->qtd] = nova;
    l->qtd++;

    return DISCIPLINA_OK; //Sucesso
}

int  disciplina_buscar(const ListaDisciplinas *l, int codigo)
{
    for (int i = 0; i < l->qtd; i++)
    {
        if (l->dados[i].codigo == codigo)
        {
            return i; // Retorna o índice da disciplina encontrada
        }
    }
    return DISCIPLINA_NAO_ENCONTRADA; // Código-disciplina não encontrado
}

void disciplina_listar(const ListaDisciplinas *l)
{
    if (l->qtd == 0)
    {
        printf("Nenhuma disciplina cadastrada.\n");
        return;
    }

    for (int i = 0; i < l->qtd; i++)
    {
        printf("Codigo: %d, Nome: %s, Creditos: %d\n",
               l->dados[i].codigo,
               l->dados[i].nome,
               l->dados[i].creditos);
    }
}

