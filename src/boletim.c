#include <stdio.h>
#include "boletim.h"
#include "matricula.h"
#include "aluno.h"


float boletim_somar_recursivo(const float v[], int n)
{
    if (n <= 0) {
        return 0.0f; // Caso base: soma de um vetor vazio é 0
    } else {
        return v[n - 1] + boletim_somar_recursivo(v, n - 1); // Soma o último elemento e chama recursivamente para o restante
    }
}

float       boletim_media(const float v[], int n)
{
    if (n <= 0) {
        return 0.0f; // Caso base: média de um vetor vazio é 0
    } else {
        return boletim_somar_recursivo(v, n) / n; // Calcula a média usando a soma recursiva
    }
}

Situacao    boletim_situacao(float media, int qtdNotasLancadas)
{
    if (qtdNotasLancadas == 0)
        return SIT_SEM_NOTAS; // Sem notas lançadas

    if (qtdNotasLancadas < MIN_NOTAS_FINAL)
        return SIT_PARCIAL;
    
    if (media >= NOTA_APROVACAO) 
        return SIT_APROVADO;

    if (media >= NOTA_RECUPERACAO) 
        return SIT_RECUPERACAO;
    else 
        return SIT_REPROVADO;
}



const char *boletim_texto_situacao(Situacao s) 
{
    switch (s) {
        case SIT_APROVADO:
            return "Aprovado";
        case SIT_RECUPERACAO:
            return "Recuperacao";
        case SIT_REPROVADO:
            return "Reprovado";
        case SIT_PARCIAL:
            return "Parcial";
        case SIT_SEM_NOTAS:
            return "-";
        
        default:
            return "Situacao desconhecida";
    }
}

int boletim_emitir(FILE *saida, const Aluno *aluno,
                    const ListaDisciplinas *disciplinas,
                    const ListaMatriculas *matriculas,
                    int semestre)
{
    if (saida == NULL || aluno == NULL || disciplinas == NULL || matriculas == NULL) {
        return BOLETIM_ERRO_PARAMETRO; // Parâmetro inválido
    }

    int cont = 0;

    fprintf(saida, "Aluno: %s (RA: %d), Semestre: %d\n", aluno->nome, aluno->ra, semestre);
    fprintf(saida, "%-*s  %-7s  %s\n", TAM_NOME_DISC, "Disciplina", "Media", "Situacao");

    for (int i = 0; i < matriculas->qtd; i++) {
        const Matricula *m = &matriculas->dados[i];

        if (m->raAluno != aluno->ra || m->semestre != semestre) {
            continue;
        }

        const char *nome;
        char aviso[50];

        int indx_disciplina = disciplina_buscar(disciplinas, m->codDisciplina);

        if (indx_disciplina != DISCIPLINA_NAO_ENCONTRADA) {
            nome = disciplinas->dados[indx_disciplina].nome;
        } else {
            snprintf(aviso, sizeof(aviso), "(codigo %d nao encontrada)", m->codDisciplina);
            nome = aviso;   
        }

            float media = boletim_media(m->notas, m->qtdNotas);
            Situacao situacao = boletim_situacao(media, m->qtdNotas);

            char media_txt[8];
            if (m->qtdNotas == 0) {
                snprintf(media_txt, sizeof(media_txt), "-");
            } else {
                snprintf(media_txt, sizeof(media_txt), "%.2f", media);
            }

            fprintf(saida, "%-*s  %-7s  %s\n", TAM_NOME_DISC, nome, media_txt, boletim_texto_situacao(situacao));
            cont++;
        }

        if (cont == 0) {
            fprintf(saida, "Nenhuma matricula encontrada para o aluno RA: %d no semestre: %d\n", aluno->ra, semestre);
        }

        return BOLETIM_OK; // Boletim emitido com sucesso
    }