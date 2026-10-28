#include <stdio.h>
#include <string.h>
#include <stdlib.h>  
#include "utils.h"

static void encerrarPorEOF(void)
{
    fprintf(stderr, "\nEntrada encerrada. Finalizando o programa.\n");
    exit(EXIT_FAILURE);
}

void limparBuffer(void)
{
        int a;
        do
        {
            a = getchar();
        }
        while (a != '\n' && a != EOF);
}

int lerInteiro(const char *msg, int min, int max)
{
    int inteiro = 0;
    int valid;
    
    do
    {
        printf("%s", msg);
        valid = scanf("%d", &inteiro);

        if (valid == EOF)
        encerrarPorEOF();

        if (valid != 1)
            printf("Informe um valor valido\n");
        else if (inteiro > max || inteiro < min)
            printf("Numero precisa estar entre %d e %d\n", min, max);
        
        limparBuffer();
    } while (valid != 1 || inteiro > max || inteiro < min);

    return inteiro;
    
}

float lerFloat(const char *msg, float min, float max)
{
    float number = 0;
    int valid;
    
    do
    {
        printf("%s", msg);
        valid = scanf("%f", &number);

        if (valid == EOF)
            encerrarPorEOF();

        if (valid != 1)
            printf("Informe um valor valido\n");
        else if (number > max || number < min)
            printf("Numero precisa estar entre %.2f e %.2f\n", min, max);
        
        limparBuffer();
    } while (valid != 1 || number > max || number < min);

    return number;
}

void lerString(const char *msg, char *destino, int tam)
{
    if(tam <= 1)
        {
            destino[0] = '\0';
            return;
        }

    printf("%s", msg);

    if(fgets(destino, tam, stdin) == NULL)
    {
        encerrarPorEOF();
    }

    if(destino[strlen(destino) - 1] != '\n')
        limparBuffer();

    destino[strcspn(destino, "\n")] = '\0';

}
