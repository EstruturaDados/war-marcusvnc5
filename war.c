#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//================== ESTRUTURA ==================
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

//================== FUNÇÕES AUXILIARES ==================
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cadastrarTerritorios(Territorio* mapa, int n) {
    printf("\n===== CADASTRO DE TERRITÓRIOS =====\n");
    for (int i = 0; i < n; i++) {
        printf("\n=== Território %d ===\n", i + 1);
        printf("Nome do território: ");
        scanf(" %29[^\n]", mapa[i].nome);
        limparBufferEntrada();
        printf("Cor do exército: ");
        scanf(" %9s", mapa[i].cor);
        limparBufferEntrada();
        printf("Quantidade de tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
    printf("\n");
}

void exibirMapa(Territorio* mapa, int n) {
    printf("================================\n");
    printf("  MAPA DO MUNDO - ESTADO ATUAL  \n");
    printf("================================\n");
    for (int i = 0; i < n; i++) {
        printf("Território %d: %s | Exército: %s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("\n");
}

int todosMesmoExercito(Territorio *mapa, int n) {
    if (n <= 0) return 1;
    for (int i = 1; i < n; i++) {
        if (strcmp(mapa[i].cor, mapa[0].cor) != 0) return 0;
    }
    return 1;
}

//================== FUNÇÕES DE MISSÃO ==================

//Sorteia missão de derrotar um exército específico
void atribuirMissao(char* destino, char** cores, int total, char* corJogador) {
    int indice;
    do {
        indice = rand() % total;
    } while (strcmp(cores[indice], corJogador) == 0);

    sprintf(destino, "Derrotar completamente o exército %s.", cores[indice]);
}

void exibirMissao(char* missao) {
    printf("%s\n", missao);
}

//Verifica se a missão foi cumprida (exército-alvo não existe mais no mapa)
int verificarMissao(char* missao, Territorio* mapa, int n) {
    char alvo[30];
    if (sscanf(missao, "Derrotar completamente o exército %29[^.].", alvo) != 1)
        return 0;

    for (int i = 0; i < n; i++) {
        if (strcasecmp(mapa[i].cor, alvo) == 0)
            return 0; //ainda existe território com essa cor
    }
    return 1; //missão cumprida
}

//================== FUNÇÃO DE ATAQUE ==================
void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n=== BATALHA ENTRE %s (Exército %s) e %s (Exército %s) ===\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);

    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;

    printf("O atacante rolou: %d | O defensor rolou: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf(">> %s perdeu 1 tropa!\n", defensor->nome);
        defensor->tropas -= 1;
        if (defensor->tropas <= 0) {
            printf(">> %s foi conquistado pelo exército %s!\n", defensor->nome, atacante->cor);
            strcpy(defensor->cor, atacante->cor);
            defensor->tropas = 1; //ganha território com 1 tropa
        }
    } else if (dadoDefensor > dadoAtacante) {
        atacante->tropas -= 1;
        if (atacante->tropas < 1) atacante->tropas = 1;
        printf(">> %s perdeu 1 tropa (restam %d).\n", atacante->nome, atacante->tropas);
    } else {
        printf("Empate! Nenhum perde tropa.\n");
    }
}

//================== LIBERAÇÃO DE MEMÓRIA ==================
void liberarMemoria(Territorio* mapa, int n, char** missoes, int numJogadores, char** coresJogadores) {
    free(mapa);
    for (int i = 0; i < numJogadores; i++) {
        free(missoes[i]);
        free(coresJogadores[i]);
    }
    free(missoes);
    free(coresJogadores);
    printf("\nMemória liberada com sucesso!\n");
}

//================== FUNÇÃO PRINCIPAL ==================
int main() {
    srand((unsigned) time(NULL));

    printf("======================================\n");
    printf("  WAR ESTRUTURADO - CADASTRO INICIAL  \n");
    printf("======================================\n");

    int n;
    printf("Digite o número de territórios do mapa: ");
    while (scanf("%d", &n) != 1 || n <= 0) {
        limparBufferEntrada();
        printf("Entrada inválida. Digite um número inteiro positivo: ");
    }
    limparBufferEntrada();

    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    if (!mapa) { printf("Erro ao alocar memória!\n"); return 1; }

    cadastrarTerritorios(mapa, n);

    //Identificar cores únicas (jogadores)
    char** coresJogadores = (char**) malloc(n * sizeof(char*));
    int numJogadores = 0;
    for (int i = 0; i < n; i++) {
        int existe = 0;
        for (int j = 0; j < numJogadores; j++) {
            if (strcasecmp(mapa[i].cor, coresJogadores[j]) == 0) { existe = 1; break; }
        }
        if (!existe) {
            coresJogadores[numJogadores] = (char*) malloc(strlen(mapa[i].cor) + 1);
            strcpy(coresJogadores[numJogadores], mapa[i].cor);
            numJogadores++;
        }
    }

    //Criar missões
    char** missoesJogadores = (char**) malloc(numJogadores * sizeof(char*));
    for (int i = 0; i < numJogadores; i++) {
        missoesJogadores[i] = (char*) malloc(200 * sizeof(char));
        atribuirMissao(missoesJogadores[i], coresJogadores, numJogadores, coresJogadores[i]);
    }

    printf("\nMissões geradas com sucesso! (Use a opção 2 para vê-las.)\n");

    //================= LOOP PRINCIPAL =================
    while (1) {
        exibirMapa(mapa, n);

        if (todosMesmoExercito(mapa, n)) {
            printf("\nFIM: todos os territórios pertencem ao exército '%s'.\n", mapa[0].cor);
            break;
        }

        printf("=== MENU ===\n1 - Atacar\n2 - Verificar missões\n3 - Sair\nEscolha: ");
        int opc;
        if (scanf("%d", &opc) != 1) { limparBufferEntrada(); continue; }
        limparBufferEntrada();

        if (opc == 3) {
            printf("Liberando memória e saindo...\n");
            break;
        }

        if (opc == 2) {
            printf("\n--- MISSÕES ATUAIS ---\n");
            for (int i = 0; i < numJogadores; i++) {
                printf("Jogador (%s): ", coresJogadores[i]);
                exibirMissao(missoesJogadores[i]);
            }

            int venceu = 0;
            for (int i = 0; i < numJogadores; i++) {
                if (verificarMissao(missoesJogadores[i], mapa, n)) {
                    printf("\nJogador '%s' cumpriu sua missão: %s\n",
                           coresJogadores[i], missoesJogadores[i]);
                    venceu = 1;
                }
            }
            if (!venceu) printf("Nenhum jogador cumpriu sua missão ainda.\n");
            printf("-----------------------\n");
            continue;
        }

        if (opc == 1) {
            int idxAtacante, idxDefensor;
            printf("Território atacante (1 a %d): ", n);
            if (scanf("%d", &idxAtacante) != 1) { limparBufferEntrada(); continue; }
            printf("Território defensor (1 a %d): ", n);
            if (scanf("%d", &idxDefensor) != 1) { limparBufferEntrada(); continue; }
            limparBufferEntrada();

            idxAtacante--; idxDefensor--;
            if (idxAtacante < 0 || idxAtacante >= n || idxDefensor < 0 || idxDefensor >= n) {
                printf("Índices inválidos!\n"); continue;
            }
            if (strcasecmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                printf("Não pode atacar o próprio exército!\n"); continue;
            }

            atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

            //Após um ataque, verificar se alguém venceu
            for (int i = 0; i < numJogadores; i++) {
                if (verificarMissao(missoesJogadores[i], mapa, n)) {
                    printf("\nJogador '%s' cumpriu sua missão: %s\n",
                           coresJogadores[i], missoesJogadores[i]);
                    liberarMemoria(mapa, n, missoesJogadores, numJogadores, coresJogadores);
                    return 0;
                }
            }
        }
    }

    liberarMemoria(mapa, n, missoesJogadores, numJogadores, coresJogadores);
    return 0;
}