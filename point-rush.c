#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define CLEAR_SCREEN "clear"
#define MAX_NAME 50

// Estrutura para armazenar nome e tempo do jogador
typedef struct {
    char name[MAX_NAME];
    int time;
} Score;

int WIDTH, HEIGHT, TOTAL_FRAGMENTS, REQUIRED_FRAGMENTS, TIME_LIMIT, CORRUPTION_COUNT;
static char map[50][50];
int playerX = 0, playerY = 0;
int collectedFragments = 0;
int gameOver = 0;
time_t startTime, totalTime;
int totalElapsedTime = 0;
char playerName[MAX_NAME];

// Captura uma tecla sem Enter
int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Inicializa o mapa
void initializeMap() {
    srand(time(NULL));
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            map[y][x] = '.';
    map[playerY][playerX] = 'B';

    int placed = 0;
    while (placed < TOTAL_FRAGMENTS) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        if (map[y][x] == '.') {
            map[y][x] = 'C';
            placed++;
        }
    }
}

// Exibe o mapa
void displayMap() {
    system(CLEAR_SCREEN);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++){
            char title = map[y][x];
            if (title == 'B') printf("\033[32m%c \033[0m", title);              // Verde para o jogador
            else if (title == 'C') printf("\033[33m%c \033[0m", title);         // Amarelo para fragmentos
            else if (title == 'X') printf("\033[31m%c \033[0m", title);         // Vermelho para corrupção
            else printf("%c ", title);                                          // Normal para o resto
        }
        printf("\n");
    }
    int elapsed = (int)(time(NULL) - startTime);
    int remaining = TIME_LIMIT - elapsed;
    
    printf("\nTempo restante: %d segundos\n", remaining > 0 ? remaining : 0);
    printf("Fragmentos coletados: %d/%d\n", collectedFragments, REQUIRED_FRAGMENTS);
    printf("Use WASD para se mover ou as setas. Aperte 'Q' para desistir.\n");
}

// Movimenta o jogador
void movePlayer(char direction) {
    int newX = playerX, newY = playerY;
    
    switch (direction) {
        case 'w': case 'W': newY--; break; // Vai para cima
        case 's': case 'S': newY++; break; // Vai para baixo
        case 'a': case 'A': newX--; break; // Vai para esquerda
        case 'd': case 'D': newX++; break; // Vai para direita
        default: return;
    }
    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) return;
    if (map[newY][newX] == 'C') collectedFragments++;
    if (map[newY][newX] == 'X') {
        gameOver = 1;
        return;
    } //Mostra onde o Player esta e substitui com '.' a ultima possição do Player
    map[playerY][playerX] = '.';
        playerX = newX;
        playerY = newY;
    map[playerY][playerX] = 'B';
}

// Espalha corrupção
void spreadCorruption() {
    int spread = 0;
    while (spread < CORRUPTION_COUNT) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        if (map[y][x] == '.' && (rand() % 100) < 15) {
            map[y][x] = 'X';
            spread++;
        }
    }
}

int noMoreMoves() {
    int directions[4][2] = {
        {0, -1},  // Cima
        {0, 1},   // Baixo
        {-1, 0},  // Esquerda
        {1, 0}    // Direita
    };

    for (int i = 0; i < 4; i++) {
        int newX = playerX + directions[i][0];
        int newY = playerY + directions[i][1];

        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
            if (map[newY][newX] != 'X') {
                return 0; // Ainda há um movimento possível
            }
        }
    }

    return 1; // Todas as direções estão bloqueadas
}

// Verifica se venceu
int checkWin() {
    return collectedFragments >= REQUIRED_FRAGMENTS;
}

// Define os parâmetros do nível
void setLevel(int level) {
    switch (level) {
        case 1: WIDTH = 10; HEIGHT = 10; TOTAL_FRAGMENTS = 2; REQUIRED_FRAGMENTS = 2; TIME_LIMIT = 100; CORRUPTION_COUNT = 1; break;
        case 2: WIDTH = 20; HEIGHT = 20; TOTAL_FRAGMENTS = 5; REQUIRED_FRAGMENTS = 5; TIME_LIMIT = 90; CORRUPTION_COUNT = 2; break;
        case 3: WIDTH = 30; HEIGHT = 30; TOTAL_FRAGMENTS = 7; REQUIRED_FRAGMENTS = 7; TIME_LIMIT = 60; CORRUPTION_COUNT = 3; break;
        case 4: WIDTH = 50; HEIGHT = 30; TOTAL_FRAGMENTS = 10; REQUIRED_FRAGMENTS = 10; TIME_LIMIT = 45; CORRUPTION_COUNT = 5; break;
        default: break;
    }
}

// Salva o nome e tempo do jogador em formato simples
void saveTimeToFile(int timeInSeconds) {
    FILE *file = fopen("highscores.txt", "a");
    if (file == NULL) {
        return;
    }
    // Salva em uma linha: Nome inteiro (com espaços) seguido do tempo
    fprintf(file, "%s|%d\n", playerName, timeInSeconds);
    fclose(file);
}

// Compara dois scores (menor tempo vem primeiro)
int compareScores(const void *a, const void *b) {
    return ((Score*)a)->time - ((Score*)b)->time;
}
// Exibe os 10 melhores jogadores
void displayHighScores() {
    printf("Top 10 Jogadores:\n\n");
    FILE *file = fopen("highscores.txt", "r");
    if (!file) {
        printf("Nenhum tempo salvo ainda.\n");
        return;
    }
    Score scores[100];
    int count = 0;
    char line[100];
    // Lê cada linha, separando nome e tempo com delimitador '|' key
    while (fgets(line, sizeof(line), file)) {
        char *sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        strncpy(scores[count].name, line, MAX_NAME);
        scores[count].name[MAX_NAME - 1] = '\0'; // Segurança
        scores[count].time = atoi(sep + 1);
        count++;
        if (count >= 100) break;
    }
    fclose(file);
    // Ordena os tempos
    qsort(scores, count, sizeof(Score), compareScores);
    // Mostra os 10 primeiros
    int top = count < 10 ? count : 10;
    for (int i = 0; i < top; i++) {
        printf("%dº-%s | %d segundos\n", i + 1, scores[i].name, scores[i].time);
    }
}

// Pergunta se o jogador quer continuar (Y/N), usando switch
int askToRestart() {
    displayHighScores();
    char c;
    do {
        printf("\nDeseja jogar novamente? (Y/N): ");
        c = getch();
        switch (c) {
            case 'y': case 'Y': return 1; // Jogar novamente
            case 'n': case 'N': return 0; // Não jogar novamente
            default:
                // Entrada inválida, continua no loop
                system(CLEAR_SCREEN);
                displayHighScores();
                printf("\nEntrada inválida. Use apenas 'Y' ou 'N'\n");
        }
    } while (1); // Loop infinito até uma entrada válida
}
//Tutorial do Jogo
void showTutorial() {
    {system(CLEAR_SCREEN);
    printf("=========== TUTORIAL ===========\n\n");

    printf("Objetivo:\n");
    printf("- Colete todos os fragmentos ('C') no mapa antes que o tempo acabe\n");
    printf("- Evite a corrupção ('X') ou você perderá o jogo\n\n");

    printf("Controles:\n");
    printf("- Use as teclas WASD ou as Setas para mover\n");
    printf("- Pressione 'Q' a qualquer momento para desistir da partida\n\n");

    printf("Símbolos no mapa:\n");
    printf("-\033[32m B\033[0m = Você (Player)\n");
    printf("-\033[33m C\033[0m = Fragmento a ser coletado\n");
    printf("-\033[31m X\033[0m = Corrupção (evite!)\n");
    printf("- . = Espaço vazio\n\n");

    printf("Pressione qualquer tecla para continuar...\n");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // Pula 17 linhas
    printf("Recomendo que suba a tela usando o mouse até que apareça a palavra 'Tutorial'\n");
    printf("Agradeço pela compreensão");}
    getch(); // Espera o jogador ler e pressionar uma tecla
}
// Recebe nome do jogador (aceita espaços)
void getPlayerName() {
    system(CLEAR_SCREEN);
    printf("Seu NickName: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = '\0';
}
int getArrowOrWasd(void) {
    int ch = getch();
    if (ch == 27) { //ESC
        if (getch() == 91) { //'['
            switch(getch()) {
                case 'A': return 'w'; // seta cima
                case 'B': return 's'; // seta baixo
                case 'C': return 'd'; // seta direita
                case 'D': return 'a'; // seta esquerda
            }
        }
    }
    return ch; // retorna WASD ou Q
}
int main() {
    int playAgain = 1, level = 1;

    while (playAgain) {
        showTutorial();
        getPlayerName();
        setLevel(level);
        startTime = time(NULL);
        initializeMap();
        char key;
        totalElapsedTime = 0;
        //Finais possíveis
        while (!gameOver) {
            displayMap();
            key = getArrowOrWasd();
              //Quando o tempo esta esgotado
            if ((time(NULL) - startTime) >= TIME_LIMIT) {
                gameOver = 1;
                system(CLEAR_SCREEN);
                printf("Tempo esgotado! ");
                break;
            } //Quando vc desiste
            if (key == 'q' || key == 'Q') {
                gameOver = 1;
                system(CLEAR_SCREEN);
                printf("Você desistiu. ");
                break;
            } //Quando tu fica preso
            spreadCorruption();
            if (noMoreMoves()) {
                gameOver = 1;
                system(CLEAR_SCREEN);
                printf("Você ficou preso! Não há mais movimentos possíveis. ");
                break;
            }  // Quando vc ganha
            movePlayer(key);
            if (checkWin()) {
                int elapsed = (int)(time(NULL) - startTime);
                totalElapsedTime += elapsed;
                if (level == 4) {
                    system(CLEAR_SCREEN);
                    printf("Parabéns! Você venceu o jogo!\n");
                    printf("\nSeu tempo foi de %d segundos\n\n", totalElapsedTime);
                    saveTimeToFile(totalElapsedTime);
                    break;
                }
                level++;
                setLevel(level);
                startTime = time(NULL);
                initializeMap();
            }
        } // Final
        if (gameOver) {
            int elapsed = (int)(time(NULL) - startTime);
            totalElapsedTime += elapsed;
            printf("Tempo NÃO será salvo no ranking.\n\n");
        } // Status depois de reiniciar o jogo
        playAgain = askToRestart();
        if (playAgain) {
            gameOver = 0;
            collectedFragments = 0;
            playerX = 0;
            playerY = 0;
            level = 1;
            totalElapsedTime = 0;
        }
    }
    printf("\n\nObrigado por jogar!");
    return 0;
}