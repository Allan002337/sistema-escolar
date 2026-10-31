#ifndef DISCIPLINA_H
#define DISCIPLINA_H

#define TAM_NOME_DISC 40
#define DISCIPLINA_OK 0
#define DISCIPLINA_ERRO_REPETIDA 1
#define DISCIPLINA_ERRO_MEMORIA 2
#define DISCIPLINA_NAO_ENCONTRADA -1

typedef struct {
    int  codigo;
    char nome[TAM_NOME_DISC];
    int  creditos;
} Disciplina;

typedef struct {
    Disciplina *dados;
    int        qtd;
    int        capacidade;
} ListaDisciplinas;

void disciplina_lista_iniciar(ListaDisciplinas *l); //inicializa a lista de disciplinas, deixando-a vazia e com capacidade 0
void disciplina_lista_liberar(ListaDisciplinas *l); //libera a memória alocada para a lista de disciplinas, se houver

Disciplina disciplina_ler(const ListaDisciplinas *l); //lê os dados de uma disciplina, garantindo que o código seja único na lista
int  disciplina_cadastrar(ListaDisciplinas *l, Disciplina nova); //cadastrar uma disciplina, devolve DISCIPLINA_OK == 0, DISCIPLINA_ERRO_REPETIDA == 1, ou DISCIPLINA_ERRO_MEMORIA == 2
int  disciplina_buscar(const ListaDisciplinas *l, int codigo); //devolve o índice da disciplina ou DISCIPLINA_NAO_ENCONTRADA == -1 se não encontrada
void disciplina_listar(const ListaDisciplinas *l);

#endif