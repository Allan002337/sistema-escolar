#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   /* errno e ENOENT: para saber se o arquivo "nao existe" */

#include "arquivo.h"
#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"
#include "boletim.h"

#define TAM_LINHA 256   /* tamanho maximo de uma linha lida do arquivo texto */


/* Chamada logo depois de um fopen que deu NULL, nas funcoes de LEITURA.
   O fopen devolve NULL tanto quando o arquivo nao existe quanto quando ha
   outro problema. O errno guarda o motivo: ENOENT = "arquivo nao existe".
   (O errno tem que ser lido imediatamente, antes de chamar outra funcao.) */
static int erro_de_abertura(void)
{
    if (errno == ENOENT)
        return ARQUIVO_NAO_EXISTE;   /* primeira execucao: nao e grave */
    return ARQUIVO_ERRO_ABRIR;
}

static int fechar_gravacao(FILE *f)
{
    if (fclose(f) != 0)
        return ARQUIVO_ERRO_IO;
    return ARQUIVO_OK;
}

static long contar_registros(FILE *f)
{
    if (fseek(f, 0, SEEK_END) != 0)          /* pula para o fim do arquivo */
        return -1;

    long tamanho = ftell(f);                 /* posicao atual = tamanho em bytes */
    if (tamanho < 0)
        return -1;

    if (tamanho % (long)sizeof(Matricula) != 0)
        return -1;                           /* nao e multiplo exato: truncado */

    return tamanho / (long)sizeof(Matricula);
}


int arquivo_salvar_alunos_txt(const char *arq, const ListaAlunos *l)
{
  
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    for (int i = 0; i < l->qtd; i++)
    {
        if (strchr(l->dados[i].nome, ';') != NULL ||
            strchr(l->dados[i].curso, ';') != NULL)
            return ARQUIVO_ERRO_FORMATO;
    }

    FILE *f = fopen(arq, "w");
    if (f == NULL)
        return ARQUIVO_ERRO_ABRIR;

    for (int i = 0; i < l->qtd; i++)
    {
        if (fprintf(f, "%d;%s;%s\n",
                    l->dados[i].ra, l->dados[i].nome, l->dados[i].curso) < 0)
        {
            fclose(f);                       /* fecha mesmo no erro */
            return ARQUIVO_ERRO_IO;
        }
    }

    return fechar_gravacao(f);
}

int arquivo_carregar_alunos_txt(const char *arq, ListaAlunos *l)
{
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    if (l->qtd != 0)
        return ARQUIVO_ERRO_PARAMETRO;

    FILE *f = fopen(arq, "r");
    if (f == NULL)
        return erro_de_abertura();       

    char linha[TAM_LINHA];

    while (fgets(linha, sizeof(linha), f) != NULL)
    {
        if (strchr(linha, '\n') == NULL && !feof(f))
        {
            fclose(f);
            return ARQUIVO_ERRO_FORMATO;
        }

        linha[strcspn(linha, "\r\n")] = '\0';

        if (linha[0] == '\0')           
            continue;

        Aluno a;
        int lidos = sscanf(linha, "%d;%49[^;];%29[^\n]", &a.ra, a.nome, a.curso);

        if (lidos != 3 || a.ra < RA_MIN || a.ra > RA_MAX)
        {
            fclose(f);
            return ARQUIVO_ERRO_FORMATO;     /* linha mal formatada */
        }

        int r = aluno_cadastrar(l, a);
        if (r != ALUNO_OK)
        {
            fclose(f);
            if (r == ALUNO_ERRO_MEMORIA)
                return ARQUIVO_ERRO_MEMORIA;
            return ARQUIVO_ERRO_FORMATO;     /* RA repetido dentro do arquivo */
        }
    }

    if (ferror(f))
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    fclose(f);                               /* arquivo so lido: nao precisa conferir */
    return ARQUIVO_OK;
}

int arquivo_salvar_disciplinas_txt(const char *arq, const ListaDisciplinas *l)
{
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    /* Confere antes de abrir (o "w" apaga o arquivo antigo) */
    for (int i = 0; i < l->qtd; i++)
    {
        if (strchr(l->dados[i].nome, ';') != NULL)
            return ARQUIVO_ERRO_FORMATO;
    }

    FILE *f = fopen(arq, "w");
    if (f == NULL)
        return ARQUIVO_ERRO_ABRIR;

    for (int i = 0; i < l->qtd; i++)
    {
        if (fprintf(f, "%d;%s;%d\n",
                    l->dados[i].codigo, l->dados[i].nome, l->dados[i].creditos) < 0)
        {
            fclose(f);
            return ARQUIVO_ERRO_IO;
        }
    }

    return fechar_gravacao(f);
}

int arquivo_carregar_disciplinas_txt(const char *arq, ListaDisciplinas *l)
{
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    if (l->qtd != 0)                         /* lista deve estar vazia */
        return ARQUIVO_ERRO_PARAMETRO;

    FILE *f = fopen(arq, "r");
    if (f == NULL)
        return erro_de_abertura();

    char linha[TAM_LINHA];

    while (fgets(linha, sizeof(linha), f) != NULL)
    {
        if (strchr(linha, '\n') == NULL && !feof(f))   /* linha longa demais */
        {
            fclose(f);
            return ARQUIVO_ERRO_FORMATO;
        }

        linha[strcspn(linha, "\r\n")] = '\0';

        if (linha[0] == '\0')
            continue;

        /* %d;%39[^;];%d  ->  codigo ; nome (ate 39 caracteres) ; creditos */
        Disciplina d;
        int lidos = sscanf(linha, "%d;%39[^;];%d", &d.codigo, d.nome, &d.creditos);

        if (lidos != 3 || d.codigo <= 0 || d.creditos <= 0)
        {
            fclose(f);
            return ARQUIVO_ERRO_FORMATO;
        }

        int r = disciplina_cadastrar(l, d);
        if (r != DISCIPLINA_OK)
        {
            fclose(f);
            if (r == DISCIPLINA_ERRO_MEMORIA)
                return ARQUIVO_ERRO_MEMORIA;
            return ARQUIVO_ERRO_FORMATO;     /* codigo repetido no arquivo */
        }
    }

    if (ferror(f))
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    fclose(f);
    return ARQUIVO_OK;
}

int arquivo_salvar_matriculas_bin(const char *arq, const ListaMatriculas *l)
{
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    FILE *f = fopen(arq, "wb");              /* "wb": escreve binario, apagando o antigo */
    if (f == NULL)
        return ARQUIVO_ERRO_ABRIR;

    if (l->qtd > 0)
    {
        if (fwrite(l->dados, sizeof(Matricula), (size_t)l->qtd, f) != (size_t)l->qtd)
        {
            fclose(f);
            return ARQUIVO_ERRO_IO;
        }
    }

    return fechar_gravacao(f);
}

int arquivo_carregar_matriculas_bin(const char *arq, ListaMatriculas *l)
{
    if (arq == NULL || l == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    if (l->qtd != 0)                         /* lista deve estar vazia */
        return ARQUIVO_ERRO_PARAMETRO;

    FILE *f = fopen(arq, "rb");
    if (f == NULL)
        return erro_de_abertura();           /* primeira execucao: NAO_EXISTE */

    /* Descobre quantos registros existem usando o proprio arquivo aberto */
    long qtd = contar_registros(f);
    if (qtd < 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_FORMATO;         /* arquivo truncado/corrompido */
    }

    if (qtd == 0)                            /* arquivo vazio: nada a carregar */
    {
        fclose(f);
        return ARQUIVO_OK;
    }

    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    Matricula *novos = malloc((size_t)qtd * sizeof(Matricula));
    if (novos == NULL)
    {
        fclose(f);
        return ARQUIVO_ERRO_MEMORIA;
    }

    /* Le tudo de uma vez e confere se veio a quantidade esperada */
    if (fread(novos, sizeof(Matricula), (size_t)qtd, f) != (size_t)qtd)
    {
        free(novos);                         /* nao vazar memoria no erro */
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    fclose(f);

    free(l->dados);
    l->dados = novos;
    l->qtd = (int)qtd;
    l->capacidade = (int)qtd;                /* capacidade coerente com o vetor */

    return ARQUIVO_OK;
}

int arquivo_ler_matricula_bin(const char *arq, int pos, Matricula *m)
{
    if (arq == NULL || m == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    if (pos < 0)
        return ARQUIVO_ERRO_POSICAO;

    FILE *f = fopen(arq, "rb");
    if (f == NULL)
        return erro_de_abertura();

    /* Confere se a posicao existe: precisa ser menor que a quantidade */
    long qtd = contar_registros(f);
    if (qtd < 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_FORMATO;
    }
    if (pos >= qtd)
    {
        fclose(f);
        return ARQUIVO_ERRO_POSICAO;
    }

    /* O PULO: vai direto ao inicio do registro "pos" */
    if (fseek(f, (long)pos * (long)sizeof(Matricula), SEEK_SET) != 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    /* Le UM registro (quantidade = 1) para dentro de *m */
    if (fread(m, sizeof(Matricula), 1, f) != 1)
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    fclose(f);
    return ARQUIVO_OK;
}

int arquivo_atualizar_matricula_bin(const char *arq, int pos, const Matricula *m)
{
    if (arq == NULL || m == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    if (pos < 0)
        return ARQUIVO_ERRO_POSICAO;

    FILE *f = fopen(arq, "rb+");
    if (f == NULL)
        return erro_de_abertura();

    long qtd = contar_registros(f);
    if (qtd < 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_FORMATO;
    }
    if (pos >= qtd)
    {
        fclose(f);
        return ARQUIVO_ERRO_POSICAO;
    }

    if (fseek(f, (long)pos * (long)sizeof(Matricula), SEEK_SET) != 0)
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    if (fwrite(m, sizeof(Matricula), 1, f) != 1)
    {
        fclose(f);
        return ARQUIVO_ERRO_IO;
    }

    return fechar_gravacao(f);
}

int arquivo_contar_matriculas_bin(const char *arq)
{
    if (arq == NULL)
        return -1;

    FILE *f = fopen(arq, "rb");
    if (f == NULL)
        return -1;

    long qtd = contar_registros(f);          /* -1 se corrompido */
    fclose(f);

    return (int)qtd;
}

int arquivo_exportar_boletim_txt(const char *arq, const Aluno *aluno,
                                 const ListaDisciplinas *disciplinas,
                                 const ListaMatriculas *matriculas, int semestre)
{
    if (arq == NULL || aluno == NULL || disciplinas == NULL || matriculas == NULL)
        return ARQUIVO_ERRO_PARAMETRO;

    FILE *f = fopen(arq, "w");
    if (f == NULL)
        return ARQUIVO_ERRO_ABRIR;           /* ex.: a pasta data/ nao existe */

    /* Escreve o boletim dentro do arquivo */
    int r = boletim_emitir(f, aluno, disciplinas, matriculas, semestre);

    /* Fecha sempre, e confere se tudo foi para o disco */
    int fechou = fechar_gravacao(f);

    if (r != BOLETIM_OK)
        return ARQUIVO_ERRO_PARAMETRO;       /* o boletim recusou algum parametro */

    return fechou;
}