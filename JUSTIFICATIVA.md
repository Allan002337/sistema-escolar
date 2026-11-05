# Justificativa da função recursiva

## A função

```c
/* RECURSIVA: soma v[0..n-1]. Caso base: n <= 0 */
float boletim_somar_recursivo(const float v[], int n)
{
    if (n <= 0)
        return 0.0f;                                   /* caso base: vetor vazio soma 0 */

    return v[n - 1] + boletim_somar_recursivo(v, n - 1);   /* último elemento + soma do resto */
}
```

Ela está no módulo `boletim` (`src/boletim.c`) e é usada por `boletim_media`, que por sua
vez alimenta o cálculo da situação final de cada disciplina no boletim.

## Como ela funciona

A soma de um vetor tem uma definição naturalmente recursiva:

> a soma de `n` elementos é o último elemento mais a soma dos `n - 1` anteriores;
> a soma de nenhum elemento é zero.

Para `v = {7, 8}` e `n = 2`, a pilha de chamadas é:

```
somar(v, 2)  =  v[1] + somar(v, 1)
                 8   + ( v[0] + somar(v, 0) )
                         7   + (    0       )
                                    ^ caso base: n <= 0 devolve 0

resultado: 0 -> 7 -> 15 
```

Cada chamada reduz `n` em uma unidade, então o caso base é sempre alcançado.

## Por que o caso base é `n <= 0` e não `n == 0`

Se alguém chamasse a função com `n` negativo (por erro de cálculo em outro módulo), um
teste `n == 0` nunca seria verdadeiro, a recursão não terminaria e a pilha estouraria.
Com `n <= 0`, uma entrada inválida devolve 0 em vez de derrubar o programa. A função
`boletim_media` também protege a divisão: só divide por `n` quando `n > 0`.

## Alternativa iterativa

```c
float boletim_somar_iterativo(const float v[], int n)
{
    float soma = 0.0f;
    for (int i = 0; i < n; i++)
        soma += v[i];
    return soma;
}
```

## Comparação

| Critério | Recursiva | Iterativa |
| Legibilidade | Espelha a definição matemática (último + resto) | Um laço simples, também muito claro |
| Tempo | O(n): uma chamada por elemento | O(n): uma volta por elemento |
| Memória extra | O(n) de pilha: um quadro por chamada | O(1): só a variável `soma` |
| Custo por elemento | Maior (chamada de função) | Menor |
| Risco de estouro de pilha | Existe para `n` grande | Não existe |
| Resultado para os mesmos dados | Igual | Igual |

## Por que escolhemos a recursiva neste projeto

1. A estrutura do problema é recursiva. A soma de uma sequência se define a partir da
   soma de uma sequência menor, e o código reproduz essa definição quase palavra por
   palavra: um caso base e um passo que reduz o problema.
2. O risco da recursão não se aplica aqui. O vetor de notas de uma matrícula tem no
   máximo `MAX_AVALIACOES` elementos (4). A profundidade da recursão nunca passa de
   4 chamadas mais o caso base, algo irrelevante para a pilha. O mesmo raciocínio não
   valeria para um vetor de milhares de elementos.
3. O custo extra é desprezível. Com no máximo 4 elementos, a diferença de tempo e de
   memória para a versão iterativa não é perceptível.
4. Atende ao objetivo da atividade: praticar a decomposição em caso base e passo
   recursivo, com uma função pequena, fácil de testar e de acompanhar na pilha.

## Limites da escolha (honestidade técnica)

Para somar um vetor pequeno e de tamanho conhecido, a versão iterativa seria igualmente
simples e um pouco mais eficiente, e numa aplicação real seria a escolha preferível,
principalmente se o número de notas pudesse crescer sem limite. A recursão foi mantida
aqui porque o limite do vetor é pequeno e fixo, o que elimina o risco principal, e
porque o problema se presta bem a esse estilo.

Caso o sistema passasse a aceitar um número grande de avaliações por disciplina,
a troca seria trivial: bastaria substituir o corpo de `boletim_somar_recursivo` pelo laço
acima, sem alterar nenhuma outra função, já que a assinatura é a mesma.

## Como testamos

| Entrada (`v`, `n`) | Soma esperada | Observação |
|---|---|---|
| `{6, 6}`, 2 | 12 | média 6.00 → Aprovado |
| `{5, 5.5}`, 2 | 10.5 | média 5.25 → Recuperação |
| `{3, 4}`, 2 | 7 | média 3.50 → Reprovado |
| `{8}`, 1 | 8 | uma única nota → situação Parcial |
| qualquer, 0 | 0 | caso base direto |
| qualquer, -1 | 0 | entrada inválida não estoura a pilha |