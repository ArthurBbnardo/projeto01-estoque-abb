/*
 * PROJETO 01 - SISTEMA DE CADASTRO COM ARVORE BINARIA DE BUSCA
 * Tema: controle de estoque de produtos de tecnologia
 *
 * Em memoria: todos os produtos ficam numa arvore binaria de busca (ABB),
 * ordenada pelo codigo do produto (chave unica, 5 digitos).
 * Em disco: arquivo produtos.csv, um produto por linha, campos separados
 * por ponto e virgula: codigo;descricao;marca;categoria;quantidade;preco
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_DADOS "produtos.csv"

#define TAM_DESCRICAO 60
#define TAM_MARCA 30
#define TAM_CATEGORIA 30

#define CODIGO_MINIMO 10000
#define CODIGO_MAXIMO 99999

/* resultados possiveis da insercao */
#define RESULTADO_OK 1
#define RESULTADO_DUPLICADO 0
#define RESULTADO_SEM_MEMORIA -1

/* ------------------------------------------------------------------ */
/* ESTRUTURAS                                                          */
/* ------------------------------------------------------------------ */

/* O registro: um produto do estoque. O codigo e a chave da arvore. */
typedef struct {
    int codigo;
    char descricao[TAM_DESCRICAO];
    char marca[TAM_MARCA];
    char categoria[TAM_CATEGORIA];
    int quantidade;
    float preco;
} Produto;

/* O no da arvore: um produto completo e os ponteiros para os dois filhos. */
typedef struct No {
    Produto produto;
    struct No *esquerda;   /* codigos menores */
    struct No *direita;    /* codigos maiores */
} No;

/* ------------------------------------------------------------------ */
/* LEITURA DO TECLADO                                                  */
/* ------------------------------------------------------------------ */

/* Descarta o que sobrou na linha quando o usuario digita mais que o campo aceita. */
void limparRestoDaLinha(void) {
    int caractere;
    do {
        caractere = getchar();
    } while (caractere != '\n' && caractere != EOF);
}

/* Le um texto com espacos. Troca ';' por ',' para nao quebrar o CSV. */
void lerTexto(const char *mensagem, char *destino, int tamanho) {
    char *quebraDeLinha;
    int i;

    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    quebraDeLinha = strchr(destino, '\n');
    if (quebraDeLinha != NULL) {
        *quebraDeLinha = '\0';
    } else {
        limparRestoDaLinha();
    }
    for (i = 0; destino[i] != '\0'; i++) {
        if (destino[i] == ';') {
            destino[i] = ',';
        }
    }
}

/* Le um numero inteiro; repete a pergunta enquanto o usuario digitar lixo. */
int lerInteiro(const char *mensagem) {
    char linha[64];
    int valor;

    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            return 0;
        }
        if (sscanf(linha, "%d", &valor) == 1) {
            return valor;
        }
        printf("valor invalido: digite um numero inteiro\n");
    }
}

/* Le um valor com casas decimais; aceita virgula ou ponto. */
float lerDecimal(const char *mensagem) {
    char linha[64];
    float valor;
    int i;

    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            return 0.0f;
        }
        for (i = 0; linha[i] != '\0'; i++) {
            if (linha[i] == ',') {
                linha[i] = '.';
            }
        }
        if (sscanf(linha, "%f", &valor) == 1 && valor >= 0) {
            return valor;
        }
        printf("valor invalido: digite um numero maior ou igual a zero\n");
    }
}

/* ------------------------------------------------------------------ */
/* EXIBICAO DE UM PRODUTO                                              */
/* ------------------------------------------------------------------ */

void imprimirCabecalho(void) {
    printf("\n%-6s | %-32s | %-14s | %-14s | %5s | %10s\n",
           "CODIGO", "DESCRICAO", "MARCA", "CATEGORIA", "QTD", "PRECO");
    printf("-------+----------------------------------+----------------+----------------+-------+-----------\n");
}

void imprimirProduto(const Produto *produto) {
    printf("%-6d | %-32.32s | %-14.14s | %-14.14s | %5d | R$ %7.2f\n",
           produto->codigo, produto->descricao, produto->marca,
           produto->categoria, produto->quantidade, produto->preco);
}

/* ------------------------------------------------------------------ */
/* OPERACOES DA ARVORE                                                 */
/* ------------------------------------------------------------------ */

/* Aloca um no novo com uma copia do produto. Devolve NULL se faltar memoria. */
No *criarNo(Produto produto) {
    No *novo = (No *) malloc(sizeof(No));
    if (novo == NULL) {
        printf("erro: sem memoria para criar o no\n");
        return NULL;
    }
    novo->produto = produto;
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/* INSERCAO: desce pela arvore comparando as chaves ate achar o lugar vazio.
 * Chave repetida e recusada. Devolve a raiz (que so muda se a arvore estava vazia). */
No *inserir(No *raiz, Produto produto, int *resultado) {
    if (raiz == NULL) {
        No *novo = criarNo(produto);
        if (novo == NULL) {
            *resultado = RESULTADO_SEM_MEMORIA;
        } else {
            *resultado = RESULTADO_OK;
        }
        return novo;
    }
    if (produto.codigo < raiz->produto.codigo) {
        raiz->esquerda = inserir(raiz->esquerda, produto, resultado);
    } else if (produto.codigo > raiz->produto.codigo) {
        raiz->direita = inserir(raiz->direita, produto, resultado);
    } else {
        *resultado = RESULTADO_DUPLICADO;
    }
    return raiz;
}

/* BUSCA: menor vai para a esquerda, maior vai para a direita. */
No *buscar(No *raiz, int codigo) {
    if (raiz == NULL) {
        return NULL;
    }
    if (codigo == raiz->produto.codigo) {
        return raiz;
    }
    if (codigo < raiz->produto.codigo) {
        return buscar(raiz->esquerda, codigo);
    }
    return buscar(raiz->direita, codigo);
}

/* PERCURSO EM ORDEM (esquerda, raiz, direita): imprime em ordem crescente de codigo. */
void listarEmOrdem(No *raiz) {
    if (raiz == NULL) {
        return;
    }
    listarEmOrdem(raiz->esquerda);
    imprimirProduto(&raiz->produto);
    listarEmOrdem(raiz->direita);
}

/* PERCURSO EM PRE-ORDEM (raiz, esquerda, direita) com recuo por nivel. */
void exibirArvore(No *raiz, int nivel, const char *lado) {
    int i;

    if (raiz == NULL) {
        return;
    }
    for (i = 0; i < nivel; i++) {
        printf("    ");
    }
    printf("[%s] %d - %s\n", lado, raiz->produto.codigo, raiz->produto.descricao);
    exibirArvore(raiz->esquerda, nivel + 1, "esq");
    exibirArvore(raiz->direita, nivel + 1, "dir");
}

/* ALTURA: quantidade de niveis da arvore (arvore vazia tem altura 0). */
int alturaDaArvore(No *raiz) {
    int alturaEsquerda;
    int alturaDireita;

    if (raiz == NULL) {
        return 0;
    }
    alturaEsquerda = alturaDaArvore(raiz->esquerda);
    alturaDireita = alturaDaArvore(raiz->direita);
    if (alturaEsquerda > alturaDireita) {
        return alturaEsquerda + 1;
    }
    return alturaDireita + 1;
}

/* REMOCAO: tres casos - no sem filhos, com um filho, ou com dois filhos. */
No *removerNo(No *raiz, int codigo, int *removido) {
    No *filho;
    No *sucessor;

    if (raiz == NULL) {
        return NULL;
    }
    if (codigo < raiz->produto.codigo) {
        raiz->esquerda = removerNo(raiz->esquerda, codigo, removido);
    } else if (codigo > raiz->produto.codigo) {
        raiz->direita = removerNo(raiz->direita, codigo, removido);
    } else {
        *removido = 1;
        if (raiz->esquerda == NULL) {
            /* sem filhos ou so com filho da direita: o filho assume o lugar */
            filho = raiz->direita;
            free(raiz);
            return filho;
        }
        if (raiz->direita == NULL) {
            /* so com filho da esquerda: o filho assume o lugar */
            filho = raiz->esquerda;
            free(raiz);
            return filho;
        }
        /* dois filhos: o sucessor (menor codigo da subarvore direita) toma o lugar */
        sucessor = raiz->direita;
        while (sucessor->esquerda != NULL) {
            sucessor = sucessor->esquerda;
        }
        raiz->produto = sucessor->produto;
        raiz->direita = removerNo(raiz->direita, sucessor->produto.codigo, removido);
    }
    return raiz;
}

/* LISTA ESTOQUE BAIXO: percurso em ordem que so mostra quantidade <= limite.
 * Devolve quantos produtos foram mostrados. */
int listarEstoqueBaixo(No *raiz, int limite) {
    int total;

    if (raiz == NULL) {
        return 0;
    }
    total = listarEstoqueBaixo(raiz->esquerda, limite);
    if (raiz->produto.quantidade <= limite) {
        imprimirProduto(&raiz->produto);
        total++;
    }
    total += listarEstoqueBaixo(raiz->direita, limite);
    return total;
}

/* DESTRUICAO: percurso em pos-ordem (esquerda, direita, raiz).
 * Os filhos precisam ser liberados antes do pai, senao perdemos os ponteiros. */
void liberarArvore(No *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}

/* ------------------------------------------------------------------ */
/* ARQUIVO                                                             */
/* ------------------------------------------------------------------ */

/* Le o arquivo e insere cada linha na arvore. Se o arquivo nao existe,
 * a arvore comeca vazia. */
No *carregarArquivo(No *raiz) {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "r");
    Produto produto;
    int resultado = RESULTADO_OK;
    int carregados = 0;

    if (arquivo == NULL) {
        printf("arquivo %s nao encontrado: comecando com a arvore vazia\n", ARQUIVO_DADOS);
        return raiz;
    }
    /* as larguras (59, 29, 29) sao o tamanho de cada campo menos 1, por causa do '\0' */
    while (fscanf(arquivo, "%d;%59[^;];%29[^;];%29[^;];%d;%f\n",
                  &produto.codigo, produto.descricao, produto.marca,
                  produto.categoria, &produto.quantidade, &produto.preco) == 6) {
        raiz = inserir(raiz, produto, &resultado);
        if (resultado == RESULTADO_OK) {
            carregados++;
        }
    }
    fclose(arquivo);
    printf("%d produto(s) carregado(s) de %s\n", carregados, ARQUIVO_DADOS);
    return raiz;
}

/* Grava em PRE-ORDEM (raiz primeiro). Ao reler o arquivo, cada pai e inserido
 * antes dos filhos, entao a arvore volta com a mesma forma. */
void gravarPreOrdem(FILE *arquivo, No *raiz) {
    if (raiz == NULL) {
        return;
    }
    fprintf(arquivo, "%d;%s;%s;%s;%d;%.2f\n",
            raiz->produto.codigo, raiz->produto.descricao, raiz->produto.marca,
            raiz->produto.categoria, raiz->produto.quantidade, raiz->produto.preco);
    gravarPreOrdem(arquivo, raiz->esquerda);
    gravarPreOrdem(arquivo, raiz->direita);
}

/* Devolve 1 se gravou e 0 se nao conseguiu abrir o arquivo. */
int salvarArquivo(No *raiz) {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "w");

    if (arquivo == NULL) {
        printf("erro: nao foi possivel abrir %s para gravar (esta aberto em outro programa?)\n",
               ARQUIVO_DADOS);
        return 0;
    }
    gravarPreOrdem(arquivo, raiz);
    fclose(arquivo);
    return 1;
}

/* ------------------------------------------------------------------ */
/* OPCOES DO MENU                                                      */
/* ------------------------------------------------------------------ */

void mostrarMenu(void) {
    printf("\n===== ESTOQUE - H2i =====\n");
    printf("1 - Cadastrar produto\n");
    printf("2 - Buscar produto\n");
    printf("3 - Listar produtos (ordem crescente de codigo)\n");
    printf("4 - Exibir a arvore\n");
    printf("5 - Registrar entrada/saida de estoque\n");
    printf("6 - Remover produto\n");
    printf("7 - Listar produtos com estoque baixo\n");
    printf("0 - Sair (grava o arquivo)\n");
}

No *cadastrarProduto(No *raiz) {
    Produto produto;
    int resultado = RESULTADO_OK;

    produto.codigo = lerInteiro("Codigo (5 digitos): ");
    if (produto.codigo < CODIGO_MINIMO || produto.codigo > CODIGO_MAXIMO) {
        printf("codigo invalido: use 5 digitos\n");
        return raiz;
    }
    if (buscar(raiz, produto.codigo) != NULL) {
        printf("ja existe um produto com o codigo %d\n", produto.codigo);
        return raiz;
    }
    lerTexto("Descricao: ", produto.descricao, TAM_DESCRICAO);
    lerTexto("Marca: ", produto.marca, TAM_MARCA);
    lerTexto("Categoria: ", produto.categoria, TAM_CATEGORIA);
    if (produto.descricao[0] == '\0' || produto.marca[0] == '\0' || produto.categoria[0] == '\0') {
        printf("descricao, marca e categoria nao podem ficar vazias\n");
        return raiz;
    }
    produto.quantidade = lerInteiro("Quantidade em estoque: ");
    if (produto.quantidade < 0) {
        printf("a quantidade nao pode ser negativa\n");
        return raiz;
    }
    produto.preco = lerDecimal("Preco (R$): ");

    raiz = inserir(raiz, produto, &resultado);
    if (resultado == RESULTADO_OK) {
        printf("produto %d cadastrado\n", produto.codigo);
    } else if (resultado == RESULTADO_DUPLICADO) {
        printf("ja existe um produto com o codigo %d\n", produto.codigo);
    } else {
        printf("nao foi possivel cadastrar: sem memoria\n");
    }
    return raiz;
}

void buscarProduto(No *raiz) {
    int codigo = lerInteiro("Codigo a buscar: ");
    No *encontrado = buscar(raiz, codigo);

    if (encontrado == NULL) {
        printf("produto %d nao existe\n", codigo);
        return;
    }
    imprimirCabecalho();
    imprimirProduto(&encontrado->produto);
}

void listarProdutos(No *raiz) {
    if (raiz == NULL) {
        printf("nenhum produto cadastrado\n");
        return;
    }
    imprimirCabecalho();
    listarEmOrdem(raiz);
}

void mostrarArvore(No *raiz) {
    if (raiz == NULL) {
        printf("a arvore esta vazia\n");
        return;
    }
    printf("\nArvore (pre-ordem, altura %d):\n", alturaDaArvore(raiz));
    exibirArvore(raiz, 0, "raiz");
}

void registrarMovimentacao(No *raiz) {
    int codigo = lerInteiro("Codigo do produto: ");
    No *encontrado = buscar(raiz, codigo);
    int tipo;
    int quantidade;

    if (encontrado == NULL) {
        printf("produto %d nao existe\n", codigo);
        return;
    }
    printf("%s - saldo atual: %d\n", encontrado->produto.descricao, encontrado->produto.quantidade);
    tipo = lerInteiro("1 = entrada, 2 = saida: ");
    if (tipo != 1 && tipo != 2) {
        printf("tipo invalido\n");
        return;
    }
    quantidade = lerInteiro("Quantidade: ");
    if (quantidade <= 0) {
        printf("a quantidade deve ser maior que zero\n");
        return;
    }
    if (tipo == 1) {
        encontrado->produto.quantidade += quantidade;
    } else if (quantidade > encontrado->produto.quantidade) {
        printf("saida maior que o saldo: operacao cancelada\n");
        return;
    } else {
        encontrado->produto.quantidade -= quantidade;
    }
    printf("novo saldo: %d\n", encontrado->produto.quantidade);
}

No *removerProduto(No *raiz) {
    int codigo = lerInteiro("Codigo a remover: ");
    int removido = 0;

    raiz = removerNo(raiz, codigo, &removido);
    if (removido) {
        printf("produto %d removido\n", codigo);
    } else {
        printf("produto %d nao existe\n", codigo);
    }
    return raiz;
}

void mostrarEstoqueBaixo(No *raiz) {
    int limite = lerInteiro("Mostrar produtos com quantidade ate: ");
    int total;

    imprimirCabecalho();
    total = listarEstoqueBaixo(raiz, limite);
    if (total == 0) {
        printf("nenhum produto com quantidade ate %d\n", limite);
    }
}

/* ------------------------------------------------------------------ */
/* PROGRAMA PRINCIPAL                                                  */
/* ------------------------------------------------------------------ */

int main(void) {
    No *raiz = NULL;
    int opcao;
    int continuar = 1;

    raiz = carregarArquivo(raiz);

    while (continuar) {
        mostrarMenu();
        opcao = lerInteiro("Opcao: ");
        switch (opcao) {
            case 1:
                raiz = cadastrarProduto(raiz);
                break;
            case 2:
                buscarProduto(raiz);
                break;
            case 3:
                listarProdutos(raiz);
                break;
            case 4:
                mostrarArvore(raiz);
                break;
            case 5:
                registrarMovimentacao(raiz);
                break;
            case 6:
                raiz = removerProduto(raiz);
                break;
            case 7:
                mostrarEstoqueBaixo(raiz);
                break;
            case 0:
                /* so encerra se a gravacao deu certo, para nao perder os dados */
                if (salvarArquivo(raiz)) {
                    continuar = 0;
                }
                break;
            default:
                printf("opcao invalida\n");
                break;
        }
    }

    liberarArvore(raiz);
    printf("dados gravados em %s. ate logo!\n", ARQUIVO_DADOS);
    return 0;
}
