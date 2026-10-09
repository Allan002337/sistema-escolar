#include <stdio.h>
#include <string.h>
#include "utils.h"

void limparBuffer(void)
{
    int a = getchar();

    if (a != '\n' && a != EOF)
    {
        while (getchar() != '\n');
    }
}

int lerInteiro(const char *msg, int min, int max)
{
    int inteiro = 0;
    int valid;
    
    do
    {
        printf("%s", msg);
        valid = scanf("%d", &inteiro);

        if (valid != 1)
            printf("Informe um valor valido\n");
        else if (inteiro > max && inteiro < min)
            printf("Número precisa estar entre %d e %d\n", min, max);
        
        limparBuffer();
    } while (inteiro > max && inteiro < min);

    return inteiro;
    
}

float lerFloat(const char *msg, float min, float max)
{
    float number = 0;
    int valid;
    
    do
    {
        printf("%s ", msg);
        valid = scanf("%f", &number);

        if (valid != 1)
            printf("Informe um valor valido\n");
        if (number > max && number < min)
            printf("Número precisa estar entre %.2f e %.2f\n", min, max);
        
        limparBuffer();
    } while (number > max && number < min);

    return number;
}

void lerString(const char *msg, char *destino, int tam)
{
    printf("%s ", msg);
    fgets(destino, tam, stdin);
    destino[strcspn(destino, "\n")] = '\0';
}
