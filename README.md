# Projeto 01 - Sistema de Cadastro com Árvore Binária de Busca

Controle de estoque de produtos de tecnologia, em linguagem C, executado no terminal.
Trabalho individual da disciplina de Algoritmos e Estruturas de Dados (AED).

Aluno: Arthur Bernardo Santos da Silva

## O que o programa faz

Os produtos ficam em uma **árvore binária de busca** na memória, ordenada pelo código (chave única de 5 dígitos).
Em disco, ficam no arquivo `produtos.csv`: um produto por linha, campos separados por ponto e vírgula.

Formato de cada linha: `codigo;descricao;marca;categoria;quantidade;preco`

Menu:

| Opção | Operação da árvore |
|---|---|
| Cadastrar produto (recusa código repetido) | inserção |
| Buscar produto | busca |
| Listar produtos em ordem crescente de código | percurso em ordem |
| Exibir a árvore (com recuo por nível e altura) | percurso em pré-ordem |
| Registrar entrada/saída de estoque | busca + alteração de campo |
| Remover produto | remoção (0, 1 ou 2 filhos) |
| Listar produtos com estoque baixo | percurso em ordem com filtro |
| Sair (grava o arquivo e libera a memória) | gravação em pré-ordem e destruição em pós-ordem |

Ao iniciar, o programa lê o `produtos.csv` e insere cada linha na árvore. Se o arquivo não existe, a árvore começa vazia.

### Por que gravar em pré-ordem?

Ao reler o arquivo, cada pai é inserido antes dos seus filhos, então a árvore volta com a mesma forma.
Gravar em ordem crescente faria a árvore virar praticamente uma lista.

## Como compilar e executar

```
gcc -Wall -Wextra -o estoque estoque_abb.c
./estoque
```

No Dev-C++: abra `estoque_abb.c` (na mesma pasta do `produtos.csv`), compile e execute.

## Arquivos

- `estoque_abb.c`: código-fonte, em um único arquivo.
- `produtos.csv`: arquivo de dados com 12 produtos de exemplo.
