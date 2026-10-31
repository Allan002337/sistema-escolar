#ifndef ALUNO_H
#define ALUNO_H

#define TAM_NOME   50
#define TAM_CURSO  30
#define RA_MIN     1
#define RA_MAX     99999

#define ALUNO_OK              0
#define ALUNO_ERRO_REPETIDA   1
#define ALUNO_ERRO_MEMORIA    2
#define ALUNO_ERRO_NAO_ACHOU   3
#define ALUNO_INDICE_NAO_ENCONTRADA  -1

typedef struct {
    int  ra;
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
} Aluno;

typedef struct {
    Aluno *dados;
    int    qtd;
    int    capacidade;
} ListaAlunos;

void  aluno_lista_iniciar(ListaAlunos *l); //inicializa a lista de alunos, deixando-a vazia e com capacidade 0
void  aluno_lista_liberar(ListaAlunos *l); //libera a memória alocada para a lista de alunos, se houver

Aluno aluno_ler(const ListaAlunos *l); //lê os dados de um aluno, garantindo que o RA seja único na lista
int   aluno_cadastrar(ListaAlunos *l, Aluno novo); //cadastrar um aluno, devolve ALUNO_OK == 0 ou ALUNO_ERRO_REPETIDO == 1 ou ALUNO_ERRO_MEMORIA == 2
int   aluno_buscar(const ListaAlunos *l, int ra); //devolve o índice do aluno ou ALUNO_INDICE_NAO_ENCONTRADO == -1
int   aluno_remover(ListaAlunos *l, int ra); //devolve ALUNO_OK == 0 ou ALUNO_ERRO_NAO_ACHOU == 3
void  aluno_listar(const ListaAlunos *l);

#endif