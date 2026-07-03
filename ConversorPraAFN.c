#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ESTADOS 5
#define MAX_TRANSICOES 7
#define MAX_SIMBOLOS 4
#define MAX_PALAVRA 100

#define EPSILON 'e'

typedef struct {
    int numero;
    int inicial;
    int final;
    int ativo;
} Estado;

typedef struct {
    int origem;
    char simbolo;
    int destino;
} Transicao;

typedef struct {
    Estado estados[MAX_ESTADOS];
    Transicao transicoes[MAX_TRANSICOES];
    char alfabeto[MAX_SIMBOLOS];

    int qtdEstados;
    int qtdTransicoes;
    int qtdSimbolos;
    int estadoInicial;
} Automato;

void inicializarAutomato(Automato *a) {
    a->qtdEstados = 0;
    a->qtdTransicoes = 0;
    a->qtdSimbolos = 0;
    a->estadoInicial = -1;

    for (int i = 0; i < MAX_ESTADOS; i++) {
        a->estados[i].numero = -1;
        a->estados[i].inicial = 0;
        a->estados[i].final = 0;
        a->estados[i].ativo = 0;
    }
}

int buscarIndiceEstado(Automato *a, int numero) {
    for (int i = 0; i < a->qtdEstados; i++) {
        if (a->estados[i].numero == numero) {
            return i;
        }
    }

    return -1;
}

int existeNoConjunto(int conjunto[], int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (conjunto[i] == valor) {
            return 1;
        }
    }

    return 0;
}

void adicionarNoConjunto(int conjunto[], int *tamanho, int valor) {
    if (!existeNoConjunto(conjunto, *tamanho, valor)) {
        conjunto[*tamanho] = valor;
        (*tamanho)++;
    }
}

void adicionarEstado(Automato *a, int numero) {
    if (a->qtdEstados >= MAX_ESTADOS) {
        printf("Limite de estados atingido.\n");
        return;
    }

    a->estados[a->qtdEstados].numero = numero;
    a->estados[a->qtdEstados].ativo = 1;
    a->qtdEstados++;
}

void adicionarTransicao(Automato *a, int origem, char simbolo, int destino) {
    if (a->qtdTransicoes >= MAX_TRANSICOES) {
        printf("Limite de transicoes atingido.\n");
        return;
    }

    a->transicoes[a->qtdTransicoes].origem = origem;
    a->transicoes[a->qtdTransicoes].simbolo = simbolo;
    a->transicoes[a->qtdTransicoes].destino = destino;
    a->qtdTransicoes++;
}

void adicionarSimbolo(Automato *a, char simbolo) {
    if (a->qtdSimbolos >= MAX_SIMBOLOS) {
        return;
    }

    for (int i = 0; i < a->qtdSimbolos; i++) {
        if (a->alfabeto[i] == simbolo) {
            return;
        }
    }

    a->alfabeto[a->qtdSimbolos] = simbolo;
    a->qtdSimbolos++;
}

void definirInicial(Automato *a, int numero) {
    int indice = buscarIndiceEstado(a, numero);

    if (indice == -1) {
        printf("Estado inicial invalido.\n");
        return;
    }

    a->estadoInicial = numero;
    a->estados[indice].inicial = 1;
}

void definirFinal(Automato *a, int numero) {
    int indice = buscarIndiceEstado(a, numero);

    if (indice == -1) {
        printf("Estado final invalido.\n");
        return;
    }

    a->estados[indice].final = 1;
}

int ehFinal(Automato *a, int estado) {
    int indice = buscarIndiceEstado(a, estado);

    if (indice == -1) {
        return 0;
    }

    return a->estados[indice].final;
}

void fechoEpsilon(Automato *a, int estado, int fecho[], int *tamFecho) {
    adicionarNoConjunto(fecho, tamFecho, estado);

    for (int i = 0; i < a->qtdTransicoes; i++) {
        if (a->transicoes[i].origem == estado &&
            a->transicoes[i].simbolo == EPSILON) {

            int destino = a->transicoes[i].destino;

            if (!existeNoConjunto(fecho, *tamFecho, destino)) {
                fechoEpsilon(a, destino, fecho, tamFecho);
            }
        }
    }
}

void moverComSimbolo(Automato *a, int conjunto[], int tamConjunto, char simbolo, int resultado[], int *tamResultado) {
    for (int i = 0; i < tamConjunto; i++) {
        int estadoAtual = conjunto[i];

        for (int j = 0; j < a->qtdTransicoes; j++) {
            if (a->transicoes[j].origem == estadoAtual &&
                a->transicoes[j].simbolo == simbolo) {

                adicionarNoConjunto(resultado, tamResultado, a->transicoes[j].destino);
            }
        }
    }
}

void converterAFeParaAFN(Automato *afe, Automato *afn) {
    inicializarAutomato(afn);

    for (int i = 0; i < afe->qtdEstados; i++) {
        adicionarEstado(afn, afe->estados[i].numero);
    }

    definirInicial(afn, afe->estadoInicial);

    for (int i = 0; i < afe->qtdSimbolos; i++) {
        if (afe->alfabeto[i] != EPSILON) {
            adicionarSimbolo(afn, afe->alfabeto[i]);
        }
    }

    for (int i = 0; i < afe->qtdEstados; i++) {
        int estado = afe->estados[i].numero;

        int fecho[MAX_ESTADOS];
        int tamFecho = 0;

        fechoEpsilon(afe, estado, fecho, &tamFecho);

        for (int j = 0; j < tamFecho; j++) {
            if (ehFinal(afe, fecho[j])) {
                definirFinal(afn, estado);
            }
        }

        for (int s = 0; s < afn->qtdSimbolos; s++) {
            char simbolo = afn->alfabeto[s];

            int movidos[MAX_ESTADOS];
            int tamMovidos = 0;

            moverComSimbolo(afe, fecho, tamFecho, simbolo, movidos, &tamMovidos);

            int resultadoFinal[MAX_ESTADOS];
            int tamResultadoFinal = 0;

            for (int m = 0; m < tamMovidos; m++) {
                int fechoDestino[MAX_ESTADOS];
                int tamFechoDestino = 0;

                fechoEpsilon(afe, movidos[m], fechoDestino, &tamFechoDestino);

                for (int f = 0; f < tamFechoDestino; f++) {
                    adicionarNoConjunto(resultadoFinal, &tamResultadoFinal, fechoDestino[f]);
                }
            }

            for (int r = 0; r < tamResultadoFinal; r++) {
                adicionarTransicao(afn, estado, simbolo, resultadoFinal[r]);
            }
        }
    }
}

int simboloValido(Automato *a, char simbolo) {
    for (int i = 0; i < a->qtdSimbolos; i++) {
        if (a->alfabeto[i] == simbolo) {
            return 1;
        }
    }

    return 0;
}

int lerPalavra(Automato *afn, char palavra[]) {
    int estadosAtuais[MAX_ESTADOS];
    int tamAtuais = 0;

    adicionarNoConjunto(estadosAtuais, &tamAtuais, afn->estadoInicial);

    for (int i = 0; palavra[i] != '\0'; i++) {
        char simbolo = palavra[i];

        if (!simboloValido(afn, simbolo)) {
            return -1;
        }

        int proximos[MAX_ESTADOS];
        int tamProximos = 0;

        for (int e = 0; e < tamAtuais; e++) {
            int estadoAtual = estadosAtuais[e];

            for (int t = 0; t < afn->qtdTransicoes; t++) {
                if (afn->transicoes[t].origem == estadoAtual &&
                    afn->transicoes[t].simbolo == simbolo) {

                    adicionarNoConjunto(proximos, &tamProximos, afn->transicoes[t].destino);
                }
            }
        }

        tamAtuais = 0;

        for (int p = 0; p < tamProximos; p++) {
            adicionarNoConjunto(estadosAtuais, &tamAtuais, proximos[p]);
        }

        if (tamAtuais == 0) {
            return -1;
        }
    }

    for (int i = 0; i < tamAtuais; i++) {
        if (ehFinal(afn, estadosAtuais[i])) {
            return 1;
        }
    }

    return -1;
}

void imprimirPalavraSeValida(Automato *afn, char palavra[]) {
    int resultado = lerPalavra(afn, palavra);

    if (resultado == 1) {
        printf("Palavra aceita: %s\n", palavra);
    } else {
        printf("-1\n");
    }
}

void imprimirAutomato(Automato *a, char nome[]) {
    printf("\n%s\n", nome);

    printf("Estados: ");
    for (int i = 0; i < a->qtdEstados; i++) {
        printf("%d ", a->estados[i].numero);
    }

    printf("\nEstado inicial: %d\n", a->estadoInicial);

    printf("Estados finais: ");
    for (int i = 0; i < a->qtdEstados; i++) {
        if (a->estados[i].final) {
            printf("%d ", a->estados[i].numero);
        }
    }

    printf("\nAlfabeto: ");
    for (int i = 0; i < a->qtdSimbolos; i++) {
        printf("%c ", a->alfabeto[i]);
    }

    printf("\nTransicoes:\n");
    for (int i = 0; i < a->qtdTransicoes; i++) {
        printf("%d --%c--> %d\n",
               a->transicoes[i].origem,
               a->transicoes[i].simbolo,
               a->transicoes[i].destino);
    }
}

void lerEstados(Automato *a) {
    int qtd;
    int numero;

    printf("Informe a quantidade de estados, maximo 5: ");
    scanf("%d", &qtd);

    if (qtd > MAX_ESTADOS) {
        qtd = MAX_ESTADOS;
    }

    printf("Informe os estados do automato:\n");

    for (int i = 0; i < qtd; i++) {
        scanf("%d", &numero);
        adicionarEstado(a, numero);
    }
}

void lerInicial(Automato *a) {
    int inicial;

    printf("Informe o estado inicial:\n");
    scanf("%d", &inicial);

    definirInicial(a, inicial);
}

void lerTransicoes(Automato *a) {
    int qtd;
    int origem;
    int destino;
    char simbolo;

    printf("Informe a quantidade de transicoes, maximo 7: ");
    scanf("%d", &qtd);

    if (qtd > MAX_TRANSICOES) {
        qtd = MAX_TRANSICOES;
    }

    printf("Informe a funcao programa no formato origem simbolo destino.\n");
    printf("Use e para epsilon.\n");

    for (int i = 0; i < qtd; i++) {
        scanf("%d %c %d", &origem, &simbolo, &destino);

        adicionarTransicao(a, origem, simbolo, destino);
        adicionarSimbolo(a, simbolo);
    }
}

void lerFinais(Automato *a) {
    int qtd;
    int estado;

    printf("Informe a quantidade de estados finais: ");
    scanf("%d", &qtd);

    printf("Informe os estados finais:\n");

    for (int i = 0; i < qtd; i++) {
        scanf("%d", &estado);
        definirFinal(a, estado);
    }
}

int lerPalavraDoArquivo(char nomeArquivo[], char palavra[]) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        return -1;
    }

    if (fscanf(arquivo, "%s", palavra) != 1) {
        fclose(arquivo);
        return -1;
    }

    fclose(arquivo);
    return 1;
}

int main() {
    Automato afe;
    Automato afn;
    char palavra[MAX_PALAVRA];

    inicializarAutomato(&afe);

    lerEstados(&afe);
    lerInicial(&afe);
    lerTransicoes(&afe);
    lerFinais(&afe);

    converterAFeParaAFN(&afe, &afn);

    //imprimirAutomato(&afe, "AFe original");
    imprimirAutomato(&afn, "AFN convertido");

    if (lerPalavraDoArquivo("D:\\projetos pessoais\\automatoLFA\\palavra.txt", palavra) == 1) {
        imprimirPalavraSeValida(&afn, palavra);
    } else {
        printf("Erro ao ler a palavra do arquivo.\n");
    }
    if (lerPalavraDoArquivo("D:\\projetos pessoais\\automatoLFA\\palavra.txt", palavra) == 1) {
        imprimirPalavraSeValida(&afe, palavra);
    } else {
        printf("Erro ao ler a palavra do arquivo.\n");
    }

    return 0;
}