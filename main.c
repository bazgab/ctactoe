#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- SYSTEM SPECIFIC SETUP ---
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
    void sleep_ms(int milliseconds) { Sleep(milliseconds); }
    void enableAnsi() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    void sleep_ms(int milliseconds) { usleep(milliseconds * 1000); }
    int _getch(void) {
        struct termios oldattr, newattr;
        int ch;
        tcgetattr(STDIN_FILENO, &oldattr);
        newattr = oldattr;
        newattr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
        return ch;
    }
    void enableAnsi() { } 
#endif

// --- COLORS ---
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_WHITE   "\033[37m"
#define BG_SELECTED   "\033[47m\033[30m" 

// Global State
char board[3][3];
const char PLAYER = 'X';
const char COMPUTER = 'O';
int cursorRow = 1; 
int cursorCol = 1;

// Prototypes
void resetBoard();
void drawBoard();
int checkFreeSpaces();
void computerMove();
char checkWinner();

int main() {
    char winner = ' ';
    char input;
    int gameRunning = 1;

    enableAnsi(); 
    srand(time(0));
    resetBoard();

    while(gameRunning) {
        drawBoard();

        // 1. Input
        input = _getch();

        // 2. Navigation
        if (input == 'w' && cursorRow > 0) cursorRow--;
        if (input == 's' && cursorRow < 2) cursorRow++;
        if (input == 'a' && cursorCol > 0) cursorCol--;
        if (input == 'd' && cursorCol < 2) cursorCol++;

        // 3. Action
        if (input == '\n' || input == ' ' || input == '\r' || input == 'e') {
            if (board[cursorRow][cursorCol] == ' ') {
                board[cursorRow][cursorCol] = PLAYER;

                winner = checkWinner();
                if(winner != ' ' || checkFreeSpaces() == 0) {
                    gameRunning = 0;
                } else {
                    drawBoard();
                    printf("\n     " COLOR_CYAN "Computer is thinking..." COLOR_RESET);
                    sleep_ms(600);
                    
                    computerMove();
                    winner = checkWinner();
                    if(winner != ' ' || checkFreeSpaces() == 0) {
                        gameRunning = 0;
                    }
                }
            }
        }
    }

    drawBoard();
    printf("\n");
    if(winner == PLAYER) printf(COLOR_GREEN "     *** YOU WIN! ***" COLOR_RESET "\n");
    else if(winner == COMPUTER) printf(COLOR_RED "     *** YOU LOSE! ***" COLOR_RESET "\n");
    else printf(COLOR_YELLOW "     *** IT'S A TIE! ***" COLOR_RESET "\n");
    
    printf("\n");
    return 0;
}

void resetBoard() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) board[i][j] = ' ';
    }
}

// Helper to draw specific cell parts
void printCell(int row, int col, int part) {
    int isSelected = (row == cursorRow && col == cursorCol);
    char content = board[row][col];

    if(isSelected) printf("%s", BG_SELECTED);

    if (part == 0) {      // Top of cell
        printf("       "); 
    } else if (part == 1) { // Middle of cell
        printf("   ");
        if(content == PLAYER) printf(COLOR_GREEN "%c" COLOR_RESET, content);
        else if(content == COMPUTER) printf(COLOR_RED "%c" COLOR_RESET, content);
        else printf("%c", content);
        
        if(isSelected) printf("%s", BG_SELECTED); // Re-apply BG
        printf("   ");
    } else if (part == 2) { // Bottom of cell
        printf("       ");
    }

    if(isSelected) printf("%s", COLOR_RESET);
}

void drawBoard() {
    system(CLEAR_SCREEN);
    
    printf(COLOR_BOLD COLOR_CYAN "\n     TIC-TAC-TOE" COLOR_RESET "\n");
    printf(COLOR_WHITE "     (WASD to move)" COLOR_RESET "\n\n");

    // TOP BORDER
    // 23 dashes matches the internal width (7+1+7+1+7)
    printf("     +-----------------------+\n"); 

    for(int i = 0; i < 3; i++) {
        
        // Draw the 3 lines of height for each row
        for (int part = 0; part < 3; part++) {
            printf("     |"); // Left Outer Border
            
            printCell(i, 0, part);
            printf("|"); // Inner Separator
            printCell(i, 1, part);
            printf("|"); // Inner Separator
            printCell(i, 2, part);

            printf("|"); // Right Outer Border
            printf("\n");
        }

        // INNER HORIZONTAL SEPARATOR (Only between rows 0-1 and 1-2)
        if(i < 2) {
            printf("     |-------+-------+-------|\n");
        }
    }

    // BOTTOM BORDER
    printf("     +-----------------------+\n");
}

int checkFreeSpaces() {
    int freeSpaces = 0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) if(board[i][j] == ' ') freeSpaces++;
    }
    return freeSpaces;
}

void computerMove() {
    int x, y;
    if(checkFreeSpaces() > 0) {
        do {
            x = rand() % 3;
            y = rand() % 3;
        } while (board[x][y] != ' ');
        board[x][y] = COMPUTER;
    }
}

char checkWinner() {
    // Check Rows
    for(int i = 0; i < 3; i++) {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ')
            return board[i][0];
    }
    // Check Cols
    for(int i = 0; i < 3; i++) {
        if(board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[0][i] != ' ')
            return board[0][i];
    }
    // Check Diagonals
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[0][0] != ' ')
        return board[0][0];
    if(board[0][2] == board[1][1] && board[0][2] == board[2][0] && board[0][2] != ' ')
        return board[0][2];

    return ' ';
}
