#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIMITE_VOOS 10

// Códigos ANSI para cores e estilo
#define RESET   "\033[0m"
#define VERMELHO "\033[1;31m"
#define VERDE   "\033[1;32m"
#define AMARELO "\033[1;33m"
#define AZUL    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CIANO   "\033[1;36m"
#define BRANCO  "\033[1;37m"
#define NEGRITO "\033[1m"

// Estrutura de dados
typedef struct {
    int numero;
    char companhia[20];
    char destino[30];
    int portao;
    char hora[6];
    char status[15];
} Voo;

typedef struct nodo {
    Voo voo;
    struct nodo* link;
} nodo;

// Comparar horários
int compararHora(const char* h1, const char* h2) {
    int hora1, min1, hora2, min2;
    sscanf(h1, "%d:%d", &hora1, &min1);
    sscanf(h2, "%d:%d", &hora2, &min2);
    if (hora1 != hora2) return hora1 - hora2;
    return min1 - min2;
}

// Cores para status
const char* corStatus(const char* status) {
    if (strcmp(status, "Partida") == 0) return AMARELO;
    if (strcmp(status, "Em voo") == 0) return CIANO;
    if (strcmp(status, "Aterrissado") == 0) return VERDE;
    if (strcmp(status, "Cancelado") == 0) return VERMELHO;
    if (strcmp(status, "Atrasado") == 0) return MAGENTA;
    if (strcmp(status, "Embarque") == 0) return AZUL;
    if (strcmp(status, "Desembarque") == 0) return BRANCO;
    return RESET;
}

// Inserção ordenada
void inserirOrdenado(nodo** inicio, Voo novoVoo) {
    nodo* novo = (nodo*)malloc(sizeof(nodo));
    novo->voo = novoVoo;
    novo->link = NULL;

    nodo *anterior = NULL, *aux = *inicio;
    while (aux != NULL && compararHora(novoVoo.hora, aux->voo.hora) > 0) {
        anterior = aux;
        aux = aux->link;
    }

    if (anterior == NULL) {
        novo->link = *inicio;
        *inicio = novo;
    } else {
        anterior->link = novo;
        novo->link = aux;
    }

    int contador = 0;
    aux = *inicio;
    while (aux != NULL) {
        contador++;
        aux = aux->link;
    }

    if (contador > LIMITE_VOOS) {
        nodo* remover = *inicio;
        printf(VERMELHO "\n[Voo removido]" RESET " O voo das %s para %s saiu da tabela.\n",
               remover->voo.hora, remover->voo.destino);
        *inicio = remover->link;
        free(remover);
    }
}

// Exibir painel
void exibirPainel(nodo* inicio) {
    printf("\033[H\033[J");
    printf(NEGRITO VERMELHO "\n\t\t\t\tPAINEL DE VOOS" RESET );
    printf("\n" BRANCO "|---------------------------------------------------------------------------------|\n");
    printf("| Hora  | Voo   | Companhia       | Destino              | Portão | Status        |\n");
    printf("|-------|-------|-----------------|----------------------|-------|----------------|\n" RESET);

    nodo* aux = inicio;
    while (aux != NULL) {
        printf("| %-5s | %-5d | %-15s | %-20s | %-5d | %s%-14s%s |\n",
            aux->voo.hora, aux->voo.numero, aux->voo.companhia,
            aux->voo.destino, aux->voo.portao, corStatus(aux->voo.status), aux->voo.status, RESET);
        aux = aux->link;
    }
    printf(BRANCO "|---------------------------------------------------------------------------------|\n" RESET);
}

// Mostrar voo
void mostrarVoo(nodo* inicio, int numero) {
    exibirPainel(inicio);
    printf(NEGRITO "\n\nInformações do Voo Selecionado:\n" RESET);

    nodo* aux = inicio;
    while (aux != NULL) {
        if (aux->voo.numero == numero) {
            printf("\n[Voo %d]\nHora: %s\nCompanhia: %s\nDestino: %s\nPortão: %d\nStatus: %s%s%s\n",
                   aux->voo.numero, aux->voo.hora, aux->voo.companhia,
                   aux->voo.destino, aux->voo.portao, corStatus(aux->voo.status), aux->voo.status, RESET);
            break;
        }
        aux = aux->link;
    }
    if (aux == NULL) printf(VERMELHO "\nVoo não encontrado.\n" RESET);

    printf("\nPressione ENTER para voltar ao painel...");
    getchar();
}

// Alterar status
void alterarStatus(nodo* inicio, int numero) {
    nodo* aux = inicio;
    while (aux != NULL) {
        if (aux->voo.numero == numero) {
            char opcaoStatus;
            printf(NEGRITO "\nSelecione o novo status:\n" RESET);
            printf("%s1. Embarque%s\n", AZUL, RESET);
            printf("%s2. Desembarque%s\n", BRANCO, RESET);
            printf("%s3. Em voo%s\n", CIANO, RESET);
            printf("%s4. Partida%s\n", AMARELO, RESET);
            printf("%s5. Aterrissado%s\n", VERDE, RESET);
            printf("%s6. Cancelado%s\n", VERMELHO, RESET);
            printf("%s7. Atrasado%s\n", MAGENTA, RESET);
            printf("Opção: ");
            scanf(" %c", &opcaoStatus); getchar();

            switch(opcaoStatus) {
                case '1': strcpy(aux->voo.status, "Embarque"); break;
                case '2': strcpy(aux->voo.status, "Desembarque"); break;
                case '3': strcpy(aux->voo.status, "Em voo"); break;
                case '4': strcpy(aux->voo.status, "Partida"); break;
                case '5': strcpy(aux->voo.status, "Aterrissado"); break;
                case '6': strcpy(aux->voo.status, "Cancelado"); break;
                case '7': strcpy(aux->voo.status, "Atrasado"); break;
                default: printf(VERMELHO "Opção inválida!\n" RESET); return;
            }
            printf("Status alterado para: %s%s%s\n", corStatus(aux->voo.status), aux->voo.status, RESET);
            return;
        }
        aux = aux->link;
    }
    printf(VERMELHO "Voo não encontrado.\n" RESET);
}

// Excluir voo
void excluirVoo(nodo** inicio, int numero) {
    nodo *aux = *inicio, *anterior = NULL;
    while (aux != NULL && aux->voo.numero != numero) {
        anterior = aux;
        aux = aux->link;
    }
    if (aux == NULL) {
        printf(VERMELHO "Voo não encontrado.\n" RESET);
        return;
    }
    char confirmacao;
    printf(NEGRITO "Confirmar exclusão do voo %d? (S/N): " RESET, numero);
    scanf(" %c", &confirmacao); getchar();
    if (confirmacao == 'S' || confirmacao == 's') {
        if (anterior == NULL) *inicio = aux->link;
        else anterior->link = aux->link;
        free(aux);
        printf(VERDE "Voo excluído com sucesso.\n" RESET);
    } else {
        printf(AMARELO "Exclusão cancelada.\n" RESET);
    }
}

// Preencher lista inicial
void preencherVoosIniciais(nodo** lista) {
    Voo voos[7] = {
        {1001, "AZUL", "Fortaleza", 16, "08:20", "Partida"},
        {1003, "LATAM", "Belem", 9, "11:30", "Aterrissado"},
        {1004, "AZUL", "Belem", 1, "15:30", "Partida"},
        {1006, "GOL", "Curitiba", 7, "10:00", "Aterrissado"},
        {1007, "GOL", "Salvador", 12, "11:45", "Aterrissado"},
        {1008, "LATAM", "Belem", 2, "14:30", "Em voo"},
        {1009, "AZUL", "Porto Alegre", 6, "21:00", "Cancelado"}
    };
    for (int i = 0; i < 7; i++) inserirOrdenado(lista, voos[i]);
}

int vooExiste(nodo* inicio, int numero) {
    nodo* aux = inicio;
    while (aux != NULL) {
        if (aux->voo.numero == numero) return 1;
        aux = aux->link;
    }
    return 0;
}

// Função principal do Código
int main() {
    nodo* lista = NULL;
    preencherVoosIniciais(&lista);
    char opcao;
    do {
        exibirPainel(lista);
        printf(NEGRITO "\n\n[A] Adicionar  [I] Info Voo  [S] Alterar Status  [E] Excluir  [X] Sair\n" RESET);
        printf("Opção: ");
        scanf(" %c", &opcao); getchar();

        if (opcao == 'A' || opcao == 'a') {
            Voo novo;
            do {
                printf("Número do voo: ");
                scanf("%d", &novo.numero); getchar();
                if (vooExiste(lista, novo.numero)) {
                    printf(VERMELHO "[Voo existente]" RESET " Tente outro número.\n");
                }
            } while (vooExiste(lista, novo.numero));

            printf("Companhia: "); fgets(novo.companhia, 20, stdin); strtok(novo.companhia, "\n");
            printf("Destino: "); fgets(novo.destino, 30, stdin); strtok(novo.destino, "\n");
            printf("Portão: "); scanf("%d", &novo.portao); getchar();
            printf("Hora (HH:MM): "); fgets(novo.hora, 6, stdin); getchar(); strtok(novo.hora, "\n");

            printf(NEGRITO "\nSelecione o status:\n" RESET);
            printf("%s1. Embarque%s\n", AZUL, RESET);
            printf("%s2. Desembarque%s\n", BRANCO, RESET);
            printf("%s3. Em voo%s\n", CIANO, RESET);
            printf("%s4. Partida%s\n", AMARELO, RESET);
            printf("%s5. Aterrissado%s\n", VERDE, RESET);
            printf("%s6. Cancelado%s\n", VERMELHO, RESET);
            printf("%s7. Atrasado%s\n", MAGENTA, RESET);
            printf("Opção: ");
            char opcaoStatus;
            scanf(" %c", &opcaoStatus); getchar();

            switch(opcaoStatus) {
                case '1': strcpy(novo.status, "Embarque"); break;
                case '2': strcpy(novo.status, "Desembarque"); break;
                case '3': strcpy(novo.status, "Em voo"); break;
                case '4': strcpy(novo.status, "Partida"); break;
                case '5': strcpy(novo.status, "Aterrissado"); break;
                case '6': strcpy(novo.status, "Cancelado"); break;
                case '7': strcpy(novo.status, "Atrasado"); break;
                default: printf(AMARELO "Opção inválida. Usando 'Embarque'.\n" RESET);
                         strcpy(novo.status, "Embarque");
            }

            inserirOrdenado(&lista, novo);
        }
        else if (opcao == 'I' || opcao == 'i') {
            int num;
            printf("Número do voo: "); scanf("%d", &num); getchar();
            mostrarVoo(lista, num);
        }
        else if (opcao == 'S' || opcao == 's') {
            int num;
            printf("Número do voo: "); scanf("%d", &num); getchar();
            alterarStatus(lista, num);
        }
        else if (opcao == 'E' || opcao == 'e') {
            int num;
            printf("Número do voo: "); scanf("%d", &num); getchar();
            excluirVoo(&lista, num);
        }
    } while (opcao != 'X' && opcao != 'x');

    return 0;
}
