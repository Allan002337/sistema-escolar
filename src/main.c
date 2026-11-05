#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "aluno.h"
#include "disciplina.h"
#include "matricula.h"
#include "boletim.h"
#include "arquivo.h"

#define ARQ_ALUNOS      "data/alunos.txt"
#define ARQ_DISCIPLINAS "data/disciplinas.txt"
#define ARQ_MATRICULAS  "data/matriculas.bin"

/* Faixa do codigo de disciplina: o ideal e estas constantes morarem no
   disciplina.h e serem usadas tambem no disciplina_ler. Os #ifndef evitam
   conflito caso voces as criem la depois. */
#ifndef COD_DISC_MIN
#define COD_DISC_MIN 1
#endif
#ifndef COD_DISC_MAX
#define COD_DISC_MAX 9999
#endif


/* Traduz um codigo do modulo arquivo em texto para o usuario. */
static const char *texto_erro_arquivo(int cod)
{
    switch (cod)
    {
        case ARQUIVO_ERRO_ABRIR:     return "nao foi possivel abrir o arquivo (a pasta data/ existe?)";
        case ARQUIVO_ERRO_IO:        return "falha de leitura/escrita no arquivo";
        case ARQUIVO_ERRO_FORMATO:   return "arquivo com formato invalido ou corrompido";
        case ARQUIVO_ERRO_MEMORIA:   return "memoria insuficiente";
        case ARQUIVO_ERRO_PARAMETRO: return "parametro invalido";
        case ARQUIVO_ERRO_POSICAO:   return "posicao inexistente no arquivo";
        default:                     return "erro desconhecido";
    }
}

/* Salvam uma lista e avisam se falhar. Sao chamadas depois de cada alteracao.
   Devolvem 0 se salvou e 1 se falhou (a main usa isso na mensagem final). */
static int salvar_alunos(const ListaAlunos *a)
{
    int r = arquivo_salvar_alunos_txt(ARQ_ALUNOS, a);
    if (r != ARQUIVO_OK)
    {
        printf("ERRO ao salvar alunos: %s.\n", texto_erro_arquivo(r));
        return 1;
    }
    return 0;
}

static int salvar_disciplinas(const ListaDisciplinas *d)
{
    int r = arquivo_salvar_disciplinas_txt(ARQ_DISCIPLINAS, d);
    if (r != ARQUIVO_OK)
    {
        printf("ERRO ao salvar disciplinas: %s.\n", texto_erro_arquivo(r));
        return 1;
    }
    return 0;
}

static int salvar_matriculas(const ListaMatriculas *m)
{
    int r = arquivo_salvar_matriculas_bin(ARQ_MATRICULAS, m);
    if (r != ARQUIVO_OK)
    {
        printf("ERRO ao salvar matriculas: %s.\n", texto_erro_arquivo(r));
        return 1;
    }
    return 0;
}

/* Carrega os tres arquivos ao iniciar. Devolve 0 se pode continuar e 1 se
   DEVE parar.
     - ARQUIVO_NAO_EXISTE = primeira execucao: segue com lista vazia.
     - Qualquer outro erro: o programa para SEM salvar nada. Se continuasse, o
       primeiro "salvar" gravaria por cima do arquivo (talvez so parcialmente
       lido) e os dados originais se perderiam. */
static int carregar_tudo(ListaAlunos *a, ListaDisciplinas *d, ListaMatriculas *m)
{
    int r = arquivo_carregar_alunos_txt(ARQ_ALUNOS, a);
    if (r != ARQUIVO_OK && r != ARQUIVO_NAO_EXISTE)
    {
        printf("ERRO ao carregar %s: %s.\n", ARQ_ALUNOS, texto_erro_arquivo(r));
        return 1;
    }

    r = arquivo_carregar_disciplinas_txt(ARQ_DISCIPLINAS, d);
    if (r != ARQUIVO_OK && r != ARQUIVO_NAO_EXISTE)
    {
        printf("ERRO ao carregar %s: %s.\n", ARQ_DISCIPLINAS, texto_erro_arquivo(r));
        return 1;
    }

    r = arquivo_carregar_matriculas_bin(ARQ_MATRICULAS, m);
    if (r != ARQUIVO_OK && r != ARQUIVO_NAO_EXISTE)
    {
        printf("ERRO ao carregar %s: %s.\n", ARQ_MATRICULAS, texto_erro_arquivo(r));
        return 1;
    }

    return 0;
}

/* 1 - Cadastrar aluno */
static void opcao_cadastrar_aluno(ListaAlunos *a)
{
    /* aluno_ler ja garante RA unico e nome/curso nao vazios */
    Aluno novo = aluno_ler(a);

    /* O ';' e o separador do arquivo texto: se estivesse no nome, a linha
       nao seria lida de volta. Recusamos aqui, antes de cadastrar. */
    if (strchr(novo.nome, ';') != NULL || strchr(novo.curso, ';') != NULL)
    {
        printf("Erro: nome e curso nao podem conter o caractere ';'.\n");
        return;
    }

    int r = aluno_cadastrar(a, novo);

    if (r == ALUNO_OK)
    {
        printf("Aluno cadastrado com sucesso.\n");
        salvar_alunos(a);
    }
    else if (r == ALUNO_ERRO_REPETIDO)
        printf("Erro: ja existe um aluno com esse RA.\n");
    else if (r == ALUNO_ERRO_MEMORIA)
        printf("Erro: memoria insuficiente.\n");
}

/* 3 - Remover aluno EM CASCATA: apaga tambem todas as matriculas dele.
   A ordem das etapas e: validar, pedir confirmacao, so entao apagar. */
static void opcao_remover_aluno(ListaAlunos *a, ListaMatriculas *m)
{
    int ra = lerInteiro("RA do aluno a remover: ", RA_MIN, RA_MAX);

    int idx = aluno_buscar(a, ra);
    if (idx == ALUNO_INDICE_NAO_ENCONTRADO)
    {
        printf("Aluno nao encontrado.\n");
        return;
    }

    /* Conta quantas matriculas serao apagadas, para avisar o usuario */
    int qtd_mat = 0;
    for (int i = 0; i < m->qtd; i++)
    {
        if (m->dados[i].raAluno == ra)
            qtd_mat++;
    }

    printf("Aluno: %s. Matriculas que serao apagadas junto: %d.\n",
           a->dados[idx].nome, qtd_mat);

    int confirma = lerInteiro("Confirmar a remocao? (1 = sim, 0 = nao): ", 0, 1);
    if (confirma == 0)
    {
        printf("Remocao cancelada.\n");
        return;
    }

    /* Primeiro as matriculas, depois o aluno */
    int removidas = 0;
    matricula_remover_por_aluno(m, ra, &removidas);
    aluno_remover(a, ra);

    printf("Aluno removido junto com %d matricula(s).\n", removidas);

    /* Os DOIS arquivos mudaram: salvar os dois, senao sobram matriculas
       orfas no disco e o indice das posicoes (fseek) deixa de bater. */
    salvar_matriculas(m);
    salvar_alunos(a);
}

/* 4 - Cadastrar disciplina */
static void opcao_cadastrar_disciplina(ListaDisciplinas *d)
{
    Disciplina nova = disciplina_ler(d);

    if (strchr(nova.nome, ';') != NULL)
    {
        printf("Erro: o nome nao pode conter o caractere ';'.\n");
        return;
    }

    int r = disciplina_cadastrar(d, nova);

    if (r == DISCIPLINA_OK)
    {
        printf("Disciplina cadastrada com sucesso.\n");
        salvar_disciplinas(d);
    }
    else if (r == DISCIPLINA_ERRO_REPETIDA)
        printf("Erro: ja existe uma disciplina com esse codigo.\n");
    else if (r == DISCIPLINA_ERRO_MEMORIA)
        printf("Erro: memoria insuficiente.\n");
}

/* 6 - Matricular um aluno em uma disciplina num semestre */
static void opcao_matricular(const ListaAlunos *a, const ListaDisciplinas *d,
                             ListaMatriculas *m)
{
    int ra = lerInteiro("RA do aluno: ", RA_MIN, RA_MAX);
    int ia = aluno_buscar(a, ra);
    if (ia == ALUNO_INDICE_NAO_ENCONTRADO)
    {
        printf("Aluno nao encontrado.\n");
        return;
    }

    int cod = lerInteiro("Codigo da disciplina: ", COD_DISC_MIN, COD_DISC_MAX);
    int id = disciplina_buscar(d, cod);
    if (id == DISCIPLINA_NAO_ENCONTRADA)
    {
        printf("Disciplina nao encontrada.\n");
        return;
    }

    int semestre = lerInteiro("Semestre (ex.: 20261): ", SEMESTRE_MIN, SEMESTRE_MAX);

    int r = matricula_efetuar(m, &a->dados[ia], &d->dados[id], semestre);

    if (r == MATRICULA_OK)
    {
        printf("Matricula efetuada: %s em %s (semestre %d).\n",
               a->dados[ia].nome, d->dados[id].nome, semestre);
        salvar_matriculas(m);
    }
    else if (r == MATRICULA_ERRO_REPETIDA)
        printf("Erro: o aluno ja esta matriculado nessa disciplina neste semestre.\n");
    else if (r == MATRICULA_ERRO_SEMESTRE)
        printf("Erro: semestre invalido (deve terminar em 1 ou 2, ex.: 20261).\n");
    else if (r == MATRICULA_ERRO_MEMORIA)
        printf("Erro: memoria insuficiente.\n");
    else
        printf("Erro: parametro invalido.\n");
}

/* 7 - Lancar UMA nota em uma matricula existente */
static void opcao_lancar_nota(ListaMatriculas *m)
{
    int ra = lerInteiro("RA do aluno: ", RA_MIN, RA_MAX);
    int cod = lerInteiro("Codigo da disciplina: ", COD_DISC_MIN, COD_DISC_MAX);
    int semestre = lerInteiro("Semestre (ex.: 20261): ", SEMESTRE_MIN, SEMESTRE_MAX);

    int idx = matricula_buscar(m, ra, cod, semestre);
    if (idx == MATRICULA_NAO_ENCONTRADA)
    {
        printf("Matricula nao encontrada (matricule o aluno antes).\n");
        return;
    }

    printf("Notas ja lancadas nesta matricula: %d de %d.\n",
           m->dados[idx].qtdNotas, MAX_AVALIACOES);

    /* A leitura (lerFloat) e feita aqui; matricula_lancar_nota so valida e grava */
    float nota = lerFloat("Nota (0 a 10, use ponto: 7.5): ", NOTA_MIN, NOTA_MAX);

    int r = matricula_lancar_nota(m, idx, nota);

    if (r == MATRICULA_OK)
    {
        printf("Nota lancada.\n");
        salvar_matriculas(m);
    }
    else if (r == MATRICULA_ERRO_NOTAS_CHEIAS)
        printf("Erro: essa matricula ja tem o maximo de %d notas.\n", MAX_AVALIACOES);
    else if (r == MATRICULA_ERRO_NOTA_INVALIDA)
        printf("Erro: nota fora do intervalo permitido.\n");
    else
        printf("Erro: matricula invalida.\n");
}

/* 8 - Listar as matriculas de um aluno */
static void opcao_listar_matriculas_aluno(const ListaAlunos *a, const ListaMatriculas *m)
{
    int ra = lerInteiro("RA do aluno: ", RA_MIN, RA_MAX);
    if (aluno_buscar(a, ra) == ALUNO_INDICE_NAO_ENCONTRADO)
    {
        printf("Aluno nao encontrado.\n");
        return;
    }
    matricula_listar_aluno(m, ra);
}

/* 9 - Listar os alunos matriculados em uma disciplina num semestre */
static void opcao_listar_matriculas_disciplina(const ListaDisciplinas *d, const ListaMatriculas *m)
{
    int cod = lerInteiro("Codigo da disciplina: ", COD_DISC_MIN, COD_DISC_MAX);
    if (disciplina_buscar(d, cod) == DISCIPLINA_NAO_ENCONTRADA)
    {
        printf("Disciplina nao encontrada.\n");
        return;
    }
    int semestre = lerInteiro("Semestre (ex.: 20261): ", SEMESTRE_MIN, SEMESTRE_MAX);
    matricula_listar_disciplina(m, cod, semestre);
}

/* 10 - Remover uma matricula */
static void opcao_remover_matricula(ListaMatriculas *m)
{
    int ra = lerInteiro("RA do aluno: ", RA_MIN, RA_MAX);
    int cod = lerInteiro("Codigo da disciplina: ", COD_DISC_MIN, COD_DISC_MAX);
    int semestre = lerInteiro("Semestre (ex.: 20261): ", SEMESTRE_MIN, SEMESTRE_MAX);

    int r = matricula_remover(m, ra, cod, semestre);

    if (r == MATRICULA_OK)
    {
        printf("Matricula removida.\n");
        salvar_matriculas(m);   /* regrava: o arquivo acompanha o vetor */
    }
    else
        printf("Matricula nao encontrada.\n");
}

/* 11 e 12 - Boletim na tela (saida = stdout) ou em arquivo.
   A MESMA funcao boletim_emitir serve aos dois: so muda o FILE* recebido. */
static void opcao_boletim(const ListaAlunos *a, const ListaDisciplinas *d,
                          const ListaMatriculas *m, int para_arquivo)
{
    int ra = lerInteiro("RA do aluno: ", RA_MIN, RA_MAX);
    int ia = aluno_buscar(a, ra);
    if (ia == ALUNO_INDICE_NAO_ENCONTRADO)
    {
        printf("Aluno nao encontrado.\n");
        return;
    }

    int semestre = lerInteiro("Semestre (ex.: 20261): ", SEMESTRE_MIN, SEMESTRE_MAX);

    if (!para_arquivo)
    {
        printf("\n");
        boletim_emitir(stdout, &a->dados[ia], d, m, semestre);
        return;
    }

    /* Monta o nome do arquivo: data/boletim_<ra>_<semestre>.txt
       snprintf escreve com limite de tamanho (nao estoura o vetor). */
    char caminho[64];
    snprintf(caminho, sizeof(caminho), "data/boletim_%d_%d.txt", ra, semestre);

    int r = arquivo_exportar_boletim_txt(caminho, &a->dados[ia], d, m, semestre);

    if (r == ARQUIVO_OK)
        printf("Boletim gravado em %s\n", caminho);
    else
        printf("ERRO ao gravar o boletim: %s.\n", texto_erro_arquivo(r));
}

/* 13 - Consulta DIRETA no arquivo binario, sem carregar tudo: usa o fseek
   (acesso por deslocamento). O arquivo reflete o ultimo salvamento, que
   acontece a cada alteracao. */
static void opcao_consultar_arquivo(void)
{
    int total = arquivo_contar_matriculas_bin(ARQ_MATRICULAS);

    if (total < 0)
    {
        printf("Nao foi possivel ler %s (ainda nao existe ou esta corrompido).\n", ARQ_MATRICULAS);
        return;
    }
    if (total == 0)
    {
        printf("O arquivo de matriculas esta vazio.\n");
        return;
    }

    printf("O arquivo tem %d registro(s), posicoes 0 a %d.\n", total, total - 1);
    int pos = lerInteiro("Posicao a consultar: ", 0, total - 1);

    Matricula reg;
    int r = arquivo_ler_matricula_bin(ARQ_MATRICULAS, pos, &reg);

    if (r != ARQUIVO_OK)
    {
        printf("ERRO na consulta: %s.\n", texto_erro_arquivo(r));
        return;
    }

    printf("Posicao %d -> RA: %d, Disciplina: %d, Semestre: %d, Notas lancadas: %d",
           pos, reg.raAluno, reg.codDisciplina, reg.semestre, reg.qtdNotas);
    for (int i = 0; i < reg.qtdNotas; i++)
        printf("%s%.2f", (i == 0) ? " (" : ", ", reg.notas[i]);
    if (reg.qtdNotas > 0)
        printf(")");
    printf("\n");
}

static void mostrar_menu(void)
{
    printf("\n SISTEMA ESCOLAR \n");
    printf(" 1 - Cadastrar aluno\n");
    printf(" 2 - Listar alunos\n");
    printf(" 3 - Remover aluno (e suas matriculas)\n");
    printf(" 4 - Cadastrar disciplina\n");
    printf(" 5 - Listar disciplinas\n");
    printf(" 6 - Matricular aluno em disciplina\n");
    printf(" 7 - Lancar nota\n");
    printf(" 8 - Listar matriculas de um aluno\n");
    printf(" 9 - Listar matriculas de uma disciplina\n");
    printf("10 - Remover matricula\n");
    printf("11 - Emitir boletim na tela\n");
    printf("12 - Exportar boletim para arquivo\n");
    printf("13 - Consultar matricula direto no arquivo (fseek)\n");
    printf(" 0 - Salvar e sair\n");
}

int main(void)
{
    ListaAlunos      alunos;
    ListaDisciplinas disciplinas;
    ListaMatriculas  matriculas;

    aluno_lista_iniciar(&alunos);
    disciplina_lista_iniciar(&disciplinas);
    matricula_lista_iniciar(&matriculas);

    if (carregar_tudo(&alunos, &disciplinas, &matriculas) != 0)
    {
        printf("Corrija ou apague o arquivo indicado e execute novamente.\n");
        aluno_lista_liberar(&alunos);
        disciplina_lista_liberar(&disciplinas);
        matricula_lista_liberar(&matriculas);
        return 1;
    }

    int opcao;
    do
    {
        mostrar_menu();

        opcao = lerInteiro("Escolha uma opcao: ", 0, 13);

        switch (opcao)
        {
            case 1:  opcao_cadastrar_aluno(&alunos);                                   break;
            case 2:  aluno_listar(&alunos);                                            break;
            case 3:  opcao_remover_aluno(&alunos, &matriculas);                        break;
            case 4:  opcao_cadastrar_disciplina(&disciplinas);                         break;
            case 5:  disciplina_listar(&disciplinas);                                  break;
            case 6:  opcao_matricular(&alunos, &disciplinas, &matriculas);             break;
            case 7:  opcao_lancar_nota(&matriculas);                                   break;
            case 8:  opcao_listar_matriculas_aluno(&alunos, &matriculas);              break;
            case 9:  opcao_listar_matriculas_disciplina(&disciplinas, &matriculas);    break;
            case 10: opcao_remover_matricula(&matriculas);                             break;
            case 11: opcao_boletim(&alunos, &disciplinas, &matriculas, 0);             break;
            case 12: opcao_boletim(&alunos, &disciplinas, &matriculas, 1);             break;
            case 13: opcao_consultar_arquivo();                                        break;
            case 0:  break;   /* sai do laco */
        }
    } while (opcao != 0);

    int falhas = 0;

    falhas += salvar_alunos(&alunos);
    falhas += salvar_disciplinas(&disciplinas);
    falhas += salvar_matriculas(&matriculas);

    aluno_lista_liberar(&alunos);
    disciplina_lista_liberar(&disciplinas);
    matricula_lista_liberar(&matriculas);

    if (falhas > 0)
    {
        printf("ATENCAO: nem todos os dados foram salvos. Veja as mensagens acima.\n");
        return 1;
    }

    printf("Dados salvos.\n");
    return 0;
}