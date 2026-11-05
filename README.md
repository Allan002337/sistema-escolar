# Sistema Escolar: matrícula e boletim de alunos

Sistema de console em C para gerenciar alunos, disciplinas, matrículas, notas, médias,
situação final e emissão de boletim. O usuário cadastra alunos e disciplinas, matricula
alunos em disciplinas por semestre, lança as notas de cada matrícula e emite o boletim com a
média e a situação de cada disciplina.

Trabalho da disciplina Algoritmos e Programação de Computadores II.

Integrantes: Allan Jonson e Pedro Cruz

## Como compilar

Requisito: compilador `gcc`. Todos os comandos são executados na raiz do projeto.

### Com `make` (Linux, macOS, WSL, Git Bash, MSYS2)

make          # compila e gera o executável "sistema"
make run      # compila (se preciso) e executa
make debug    # versão com verificador de memória (AddressSanitizer)
make clean    # apaga os arquivos gerados

### Sem `make` (por exemplo, Windows com MinGW)

```bash
gcc -Wall -Wextra -std=c99 -Iinclude src/main.c src/utils.c src/aluno.c src/disciplina.c src/matricula.c src/boletim.c src/arquivo.c -o sistema
```

### Executar

```bash
./sistema          # Linux/macOS/Git Bash
.\sistema.exe      # PowerShell/CMD
```

> Importante: execute sempre a partir da raiz do projeto. Os dados ficam na pasta
> `data/`, acessada por caminho relativo. Se ela não existir, crie-a (`mkdir data`) antes
> de rodar: sem ela, o programa avisa que não consegue salvar.

## Estrutura do projeto

sistema-escolar/
├── README.md
├── JUSTIFICATIVA.md      justificativa da função recursiva
├── Makefile
├── include/              cabeçalhos (.h), todos com guarda de inclusão
│   ├── utils.h
│   ├── aluno.h
│   ├── disciplina.h
│   ├── matricula.h
│   ├── boletim.h
│   └── arquivo.h
├── src/                  implementações (.c)
│   ├── main.c
│   ├── utils.c
│   ├── aluno.c
│   ├── disciplina.c
│   ├── matricula.c
│   ├── boletim.c
│   └── arquivo.c
├── data/                 arquivos de dados (gerados pelo programa)
└── build/                objetos da compilação (gerado pelo make)

## Módulos

| Módulo | Responsabilidade |
|---|---|
| `utils` | Leitura segura do teclado: inteiro, real e texto, com repetição até a entrada ser válida |
| `aluno` | `struct Aluno`, lista dinâmica de alunos: ler, cadastrar, buscar, remover e listar |
| `disciplina` | `struct Disciplina`, lista dinâmica: ler, cadastrar, buscar e listar |
| `matricula` | `struct Matricula` (vínculo aluno-disciplina-semestre com até 4 notas): efetuar, lançar nota, listar e remover |
| `boletim` | Soma recursiva, média, situação final e emissão do boletim em qualquer `FILE *` |
| `arquivo` | Persistência em arquivo texto e binário, inclusive acesso direto com `fseek` |
| `main` | Menu, validação de opções e integração dos módulos |

Os módulos se comunicam só por parâmetros e valores de retorno. As três listas são
declaradas dentro de `main()` e passadas por ponteiro; não há variáveis globais.

## Requisitos do enunciado

| Requisito | Onde está |
|---|---|
| Pelo menos um `struct` e uma coleção deles | `Aluno`, `Disciplina` e `Matricula`, em listas dinâmicas (`dados`, `qtd`, `capacidade`) |
| Pelo menos 3 módulos além do `main`, com `.h`/`.c` e guardas | `utils`, `aluno`, `disciplina`, `matricula`, `boletim` e `arquivo` |
| Nenhuma variável global | Tudo é criado em `main()` e passado por parâmetro |
| Função recursiva com justificativa | `boletim_somar_recursivo` em `src/boletim.c`; texto em [JUSTIFICATIVA.md](JUSTIFICATIVA.md) |
| Persistência em arquivo texto | `data/alunos.txt` e `data/disciplinas.txt`; boletins em `data/boletim_<ra>_<semestre>.txt` |
| Persistência em arquivo binário com acesso direto | `data/matriculas.bin`, lido e atualizado por posição com `fseek` (`arquivo_ler_matricula_bin`, `arquivo_atualizar_matricula_bin`); opção 13 do menu |
| Menu com tratamento de entrada inválida | `main.c` usando `lerInteiro`, `lerFloat` e `lerString` de `utils.c` |
| Tratamento de falha na abertura de arquivos | Todo `fopen` é testado; o `main` traduz o código de erro em mensagem |

---

## Regras do sistema

- RA: de 100000 a 999999, único por aluno. Código de disciplina: de 1000 a 9999, único.
- Matrícula: identificada por aluno + disciplina + semestre. O semestre tem o formato
  `AAAAP` (ex.: `20261` = 1º período de 2026) e deve terminar em 1 ou 2. Não é possível
  matricular o mesmo aluno duas vezes na mesma disciplina e semestre.
- Notas: de 0 a 10, lançadas uma por vez, até 4 por matrícula. Use ponto como
  separador decimal (`7.5`).
- Média: média aritmética das notas já lançadas na disciplina.
- Situação:

| Condição | Situação |
| Nenhuma nota lançada | `-` (sem notas) |
| Menos de 2 notas lançadas | Parcial |
| Média ≥ 6 | Aprovado |
| 5 ≤ Média < 6 | Recuperacao |
| Média < 5 | Reprovado |

- Remoção de aluno em cascata: ao remover um aluno, todas as matrículas dele também
  são removidas, depois de uma confirmação que mostra quantas serão apagadas.
- Salvamento automático: os dados são gravados a cada alteração e também ao sair.

## Formato dos arquivos de dados

O esboço da persistência: o que cada arquivo guarda e como.

| Arquivo | Formato | Conteúdo |
|---|---|---|
| `data/alunos.txt` | Texto | Uma linha por aluno: `ra;nome;curso` |
| `data/disciplinas.txt` | Texto | Uma linha por disciplina: `codigo;nome;creditos` |
| `data/matriculas.bin` | Binário | Registros `Matricula` de tamanho fixo, um depois do outro |
| `data/boletim_<ra>_<semestre>.txt` | Texto | Boletim legível, gerado sob demanda |

Exemplo de `alunos.txt`:


12345;Allan;Sistema de Informação
20001;Joao Lima;Ciencia da Computacao

Exemplo de `disciplinas.txt`:

1011;Calculo I;4
1021;Algoritmos II;4

Binário e acesso direto. O `matriculas.bin` não tem cabeçalho nem separadores:

[Matricula 0][Matricula 1][Matricula 2] ...
 deslocamento 0   S           2*S        (S = sizeof(Matricula))


Como todo registro tem o mesmo tamanho, o registro `pos` começa no byte
`pos * sizeof(Matricula)`, e o `fseek` vai direto a ele, sem ler os anteriores. A
quantidade de registros sai de `fseek` até o fim e `ftell` dividido por `sizeof(Matricula)`.

Se o arquivo de dados não existir (primeira execução), o programa começa com listas vazias.
Se existir, mas estiver corrompido, o programa avisa e encerra sem sobrescrevê-lo.

## Roteiro de teste

Comece com a pasta `data/` vazia (apague os arquivos que houver) e execute `./sistema`.
Cada linha abaixo indica o que digitar e o que o programa deve responder.

### 1. Menu e entrada inválida

| Entrada | Saída esperada |
| `abc` no menu | `Informe um valor valido` e o menu volta |
| `99` no menu | `Numero precisa estar entre 0 e 13` |
| Ctrl+D (Linux/macOS) ou Ctrl+Z e Enter (Windows) em qualquer pergunta | `Entrada encerrada. Finalizando o programa.` e o programa termina |

### 2. Alunos

| Entrada | Saída esperada |
| Opção `1`: RA `12345`, nome `Maria Souza`, curso `Engenharia de Software` | `Aluno cadastrado com sucesso.` |
| Opção `1` com o mesmo RA `12345` | `RA ja cadastrado. Tente novamente.` e pede outro RA |
| Opção `1` com RA `1000000` | `Numero precisa estar entre 100000 e 999999` |
| Opção `1` com nome `Joao;Lima` | `Erro: nome e curso nao podem conter o caractere ';'.` |
| Opção `2` | `Lista de Alunos:` com `RA: 12345, Nome: Maria Souza, Curso: Engenharia de Software` |

### 3. Disciplinas

| Entrada | Saída esperada |
| Opção `4`: código `1011`, nome `Calculo I`, créditos `4` | `Disciplina cadastrada com sucesso.` |
| Opção `4` com código `1011` de novo | `Codigo ja cadastrado. Tente novamente.` |
| Opção `4` com créditos `11` | `Numero precisa estar entre 1 e 10` |
| Cadastrar também `1022` Algoritmos II (4), `1031` Fisica I (4), `1042` Quimica (2) e `1054` Estatistica (3); opção `5` | Lista das cinco disciplinas |

### 4. Matrículas

| Entrada | Saída esperada |
| Opção `6`: RA `12345`, disciplina `1011`, semestre `20261` | `Matricula efetuada: Maria Souza em Calculo I (semestre 20261).` |
| Repetir a mesma matrícula | `Erro: o aluno ja esta matriculado nessa disciplina neste semestre.` |
| Opção `6` com semestre `20263` | `Erro: semestre invalido (deve terminar em 1 ou 2, ex.: 20261).` |
| Opção `6` com RA inexistente | `Aluno nao encontrado.` |
| Opção `6` com disciplina inexistente | `Disciplina nao encontrada.` |
| Matricular o RA `12345` também em `1022`, `1031`, `1042` e `1054` (semestre `20261`) | `Matricula efetuada...` para cada uma |

### 5. Notas (opção `7`, informando RA, disciplina e semestre)

| Entrada | Saída esperada |
| `1011`: notas `6` e `6` | `Nota lancada.` duas vezes |
| `1022`: notas `5` e `5.5` | `Nota lancada.` duas vezes |
| `1031`: notas `3` e `4` | `Nota lancada.` duas vezes |
| `1042`: nota `8` (uma só) | `Nota lancada.` |
| Nota `11` ou `-1` | `Numero precisa estar entre 0.00 e 10.00` e pergunta de novo |
| Quinta nota na mesma matrícula | `Erro: essa matricula ja tem o maximo de 4 notas.` |
| Matrícula que não existe | `Matricula nao encontrada (matricule o aluno antes).` |

### 6. Boletim (opção `11`, RA `12345`, semestre `20261`)

Com os dados acima, a saída esperada é:

Aluno: Maria Souza (RA: 12345), Semestre: 20261
Disciplina                                Media    Situacao
Calculo I                                 6.00     Aprovado
Algoritmos II                             5.25     Recuperacao
Fisica I                                  3.50     Reprovado
Quimica                                   8.00     Parcial
Estatistica                               -        -

Cada linha cobre um caso: média 6 aprova, 5.25 fica em recuperação, 3.50 reprova, uma
única nota gera situação parcial e nenhuma nota não mostra média.

| Entrada | Saída esperada |
| Opção `11` com semestre `20262` (sem matrículas) | Cabeçalho e `Nenhuma matricula encontrada para o aluno RA: 12345 no semestre: 20262` |
| Opção `12`, RA `12345`, semestre `20261` | `Boletim gravado em data/boletim_12345_20261.txt`, com o mesmo conteúdo do boletim acima |

### 7. Persistência e acesso direto

| Entrada | Saída esperada |
| Opção `13` | `O arquivo tem 5 registro(s), posicoes 0 a 4.` |
| Posição `1` | `Posicao 1 -> RA: 12345, Disciplina: 1022, Semestre: 20261, Notas lancadas: 2 (5.00, 5.50)` |
| Sair com `0`, executar de novo e usar as opções `2`, `5` e `8` | Os alunos, as disciplinas e as matrículas (com as notas) continuam lá |

### 8. Remoção em cascata

| Entrada | Saída esperada |
| Opção `3`, RA `12345` | `Aluno: Maria Souza. Matriculas que serao apagadas junto: 5.` |
| Confirmar com `1` | `Aluno removido junto com 5 matricula(s).` |
| Opção `13` em seguida | `O arquivo de matriculas esta vazio.` |
| Opção `3` e confirmar com `0` | `Remocao cancelada.` e nada é apagado |

### 9. Falhas de arquivo

| Situação | Saída esperada |
| Apagar a pasta `data/`, cadastrar um aluno e sair | `ERRO ao salvar alunos: nao foi possivel abrir o arquivo (a pasta data/ existe?).` e, ao sair, `ATENCAO: nem todos os dados foram salvos.` |
| Escrever `lixo` dentro de `data/alunos.txt` e iniciar | `ERRO ao carregar data/alunos.txt: arquivo com formato invalido ou corrompido.` O programa encerra e não altera o arquivo |
| Cortar o final de `data/matriculas.bin` e iniciar | `ERRO ao carregar data/matriculas.bin: arquivo com formato invalido ou corrompido.` |

---

## Decisões de projeto e limitações

- Listas dinâmicas (`malloc`/`realloc`): não há limite fixo de alunos, disciplinas ou
  matrículas. Cada lista dobra de capacidade quando enche.
- Retornos: `0` indica sucesso e valores positivos indicam erro, cada um com seu código.
  As buscas devolvem o índice ou `-1`. As mensagens de erro ficam no `main`.
- Aprovação em `>= 6`. Média exatamente 6 é aprovada.
- Média arredondada só na exibição: uma média de 5.999 aparece como `6.00`, mas é
  classificada como Recuperação.
- Ponto decimal obrigatório: digitar `7,5` é lido como `7`. Use `7.5`.
- Texto sem acentos nas mensagens, para não aparecer quebrado em terminais do Windows.
- Nome e curso não aceitam `;`, que é o separador do arquivo texto.
- Binário não é portátil: o `matriculas.bin` guarda a struct "crua". Mudar
  `MAX_AVALIACOES` ou trocar de compilador ou sistema pode invalidar arquivos antigos.
  Nesse caso, apague `data/matriculas.bin`.
- Fim de entrada (Ctrl+D ou Ctrl+Z) encerra o programa. Como os dados são salvos a cada
  alteração, só a operação em andamento é perdida.
- RA é numérico: um RA com zero à esquerda perderia o zero.