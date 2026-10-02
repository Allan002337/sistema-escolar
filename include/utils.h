#ifndef UTILS_H
#define UTILS_H

// Convencao do projeto: funcoes que podem falhar retornam 1 = sucesso ou 0 = falha. 

void  limparBuffer(void);
int   lerInteiro(const char *msg, int min, int max);       // repete ate ser valido 
float lerFloat(const char *msg, float min, float max);     // repete ate ser valido 
void  lerString(const char *msg, char *destino, int tam);  // fgets + remove '\n' 

#endif