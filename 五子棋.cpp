// A main function to start a gomoku game
// 开启一局五子棋游戏的主函数
// All code here can be modified and extended as needed
// 所有代码可根据需要进行修改和扩展
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>

// define board size
// 定义棋盘大小
#define BOARD_SIZE 15

// enum of board cells
// 棋盘格子的状态枚举
typedef enum {
    EMPTY,
    BLACK,
    WHITE
} CellState;

// enum of players
// 玩家枚举
typedef enum {
    PLAYER_BLACK,
    PLAYER_WHITE
} Player;

// enum of game modes
// 游戏模式枚举
typedef enum {
    MODE_PVP, // Player vs Player
    MODE_PVE  // Player vs AI
} GameMode;

// structure of board position
// 棋盘位置结构体
typedef struct {
    int row;
    int col; // column
} Position;

// structure of the game state can be defined here
// 游戏状态结构体
typedef struct {
    CellState board[BOARD_SIZE][BOARD_SIZE];
    Player currentPlayer; // whose turn it is
    Position lastMove; // where the last move made
} GameState;


// Function to initialize the game state
// 初始化游戏状态的函数
void initializeGame(GameState* gameState) {
    gameState->currentPlayer = PLAYER_BLACK;
    gameState->lastMove.row = -1;
    gameState->lastMove.col = -1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameState->board[i][j] = EMPTY;
        }
    }
}


// Function to print the board
// Use numbers to represent columns, characters to represent rows
// 打印棋盘的函数。使用数字表示列，字符表示行
void printBoard(GameState* gameState) {
    // Display the last move
        if (gameState->lastMove.row >= 0 && gameState->lastMove.col >= 0) {
            printf("Your last move was: %c%d\n",
                'A' + gameState->lastMove.col,
                BOARD_SIZE - gameState->lastMove.row);
        }
        else {
            printf("No moves yet.\n");
        }

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", BOARD_SIZE - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (gameState->board[i][j] == BLACK) { // is black stone
                if (j == BOARD_SIZE - 1) {
                    if (i == gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲\n");
                    else
                        printf("●\n");
                }
                else {
                    if (i == gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("▲─");
                    else
                        printf("●─");
                }
            }
            else if (gameState->board[i][j] == WHITE) { // is white stone
                if (j == BOARD_SIZE - 1) {
                    if (i == gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△\n");
                    else
                        printf("◎\n");
                }
                else {
                    if (i == gameState->lastMove.row && j == gameState->lastMove.col)
                        printf("△─");
                    else
                        printf("◎─");
                }
            }
            else if (i == 0) { // top row empty
                if (j == 0) { printf("┌─"); }
                else if (j == BOARD_SIZE - 1) { printf("┐\n"); }
                else { printf("┬─"); }
            }
            else if (i == BOARD_SIZE - 1) { // bottom row empty
                if (j == 0) { printf("└─"); }
                else if (j == BOARD_SIZE - 1) { printf("┘\n"); }
                else { printf("┴─"); }
            }
            else { // middle empty
                if (j == 0) { printf("├─"); }
                else if (j == BOARD_SIZE - 1) { printf("┤\n"); }
                else { printf("┼─"); }
            }
        }
    }

    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf(" %c", 'A' + i);
    printf("\n");
}


// Function of win checking. 0: no win, 1: black wins, 2: white wins
// 判断胜利的函数。0: 无胜利，1: 黑方胜利，2: 白方胜利
int checkWin(GameState* gameState, int row, int col) {
    // TODO: Implement win checking logic

    int directions[4][2] = { { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, -1 } };//选定四个方向判断
    for (int d = 0; d < 4; d++) { //遍历四个方向
        int count = 1;
        for (int i = 1; i < 5; i++) {
            int newRow = row + i * directions[d][0];
            int newCol = col + i * directions[d][1];
            if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE &&
                gameState->board[newRow][newCol] == gameState->board[row][col]) {//检查连子数
                count++;
            } else {
                break;
            }
        }
        for (int i = 1; i < 5; i++) {
            int newRow = row - i * directions[d][0];
            int newCol = col - i * directions[d][1];
            if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE &&
                gameState->board[newRow][newCol] == gameState->board[row][col]) { 
                count++;
            } else {
				break;//正反两个方向都检查并相加
            }
        }
        if (count >= 5) {
			return (gameState->board[row][col] == BLACK) ? 1 : 2;// 返回胜利方
        }
    }
    return 0;
}


// Function to check valid move
// 检查合法落子的函数，合理则返回1，不合理返回0
int isValidMove(GameState* gameState, int row, int col) {
    // TODO: Implement move validation logic
    if(row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && gameState->board[row][col] == EMPTY) {
        return 1; // valid
	}
    return 0;
}
//判断是否是禁手，若存在则返回1，不存在返回0
static inline int inBoard(int r, int c) {
    return (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE);
}

// 越界当作“堵”（返回 -1）
static inline int getCellSafe(const GameState* gs, int r, int c) {
    if (!inBoard(r, c)) return -1;
    return gs->board[r][c];
}

// 取一条方向线（以(row,col)为中心，取[-4..+4]共9格）
// 编码：X=黑棋，_=空，O=白棋或边界（堵）
static std::string buildLine(const GameState* gs, int row, int col, int dx, int dy) {
    std::string s;
    s.reserve(9);
    for (int k = -4; k <= 4; k++) {
        int r = row + k * dx;
        int c = col + k * dy;
        int v = getCellSafe(gs, r, c);
        if (v == -1) s.push_back('O');          // 边界堵
        else if (v == EMPTY) s.push_back('_');  // 空
        else if (v == BLACK) s.push_back('X');  // 黑
        else s.push_back('O');                  // 白当堵
    }
    return s;
}

static int countOccur(const std::string& s, const std::string& pat) {
    int cnt = 0;
    for (int i = 0; i + (int)pat.size() <= (int)s.size(); i++) {
        if (s.compare(i, pat.size(), pat) == 0) cnt++;
    }
    return cnt;
}

// 长连：出现连续 >=6 个 X
static int hasOverline(const std::string& s) {
    int run = 0;
    for (char ch : s) {
        if (ch == 'X') {
            run++;
            if (run >= 6) return 1;
        }
        else run = 0;
    }
    return 0;
}

// 活三计数
static int countOpenThree(const std::string& s) {
    int cnt = 0;
    // 经典活三
    cnt += countOccur(s, "_XXX_");
    // 跳三（常见两种）
    cnt += countOccur(s, "_XX_X_");
    cnt += countOccur(s, "_X_XX_");
    return cnt;
}

// 四威胁计数：能“一手成五”的四（用于双四禁手）
// 先覆盖最常见的几类：活四、冲四、跳四
static int countFourThreat(const std::string& s) {
    int cnt = 0;
    // 活四
    cnt += countOccur(s, "_XXXX_");
    // 冲四（至少一端空即可威胁）
    cnt += countOccur(s, "XXXX_");
    cnt += countOccur(s, "_XXXX");
    // 跳四常见形态（下一手补空可成五）
    cnt += countOccur(s, "XXX_X");
    cnt += countOccur(s, "XX_XX");
    cnt += countOccur(s, "X_XXX");
    return cnt;
}

// 黑棋禁手判断：长连 / 双三 / 双四
// 调用前提：gs->board[row][col] 已经是 BLACK（已落子）
static int isForbiddenBlackMove(const GameState* gs, int row, int col) {
    int dirs[4][2] = { {0,1},{1,0},{1,1},{1,-1} };

    int openThreeTotal = 0;
    int fourThreatTotal = 0;

    for (int d = 0; d < 4; d++) {
        std::string line = buildLine(gs, row, col, dirs[d][0], dirs[d][1]);

        // 1) 长连
        if (hasOverline(line)) return 1;

        // 2) 双三：统计活三数
        openThreeTotal += countOpenThree(line);

        // 3) 双四：统计四威胁数
        fourThreatTotal += countFourThreat(line);
    }
     
    if (openThreeTotal >= 2) return 1; // 双三
    if (fourThreatTotal >= 2) return 1; // 双四
    return 0;
}
Position AIstep(GameState* gameState) {
    Position pos;
    // 最简单的随机落子逻辑
    while (1) {
        int r = rand() % BOARD_SIZE;
        int c = rand() % BOARD_SIZE;
        if (isValidMove(gameState, r, c)) {
            // 如果是黑棋，还需要简单判断是否禁手（可选）
            pos.row = r;
            pos.col = c;
            break;
        }
    }
    return pos;
}

// Main function to start the game
// 主函数，开启游戏
int main(int argc, char* argv[]) {
    printf("Welcome to Gomoku!\nSelect mode(1: PVP, 2 : PVE) :\n" );
        
    // 初始化游戏状态
    GameState gameState;
    initializeGame(&gameState);
    GameMode gameMode = MODE_PVP;

    // Handle input before starting the game loop
    // 在开始游戏循环前处理输入
    char input[100];

    // Default to Player vs Player
    // 默认选择玩家对玩家模式
    int mode = 1;
	// AI color: 0 for Black, 1 for White
    int AIcolor =0;
	int AIturn = 0;
    // TODO: Use argc and argv to set up game mode. Default to Player vs Player.
    // 可以选择使用 argc 和 argv 来设置游戏模式，默认玩家对玩家模式。也可以选择其他方式输入。
	scanf("%d", &mode);
    if (mode == 1) {
        printf("You selected Player vs Player mode.\n");
    }
    else {
        printf("You selected Player vs AI mode.\n");
        printf("Who goes first? (1: You, 2: AI): ");
        GameMode gameMode = MODE_PVE;
        // TODO: Use stdin to ask who goes first
        // 可以选择使用标准输入来询问谁先手。 
        
    scanf("%d", &AIcolor);
        if (AIcolor == 1) {
            printf("You are Black (●), AI is White (◎).\n");
        }
        else {
            printf("AI is Black (●), You are White (◎).\n");
			AIcolor = 0;
			AIturn = 1;
		}
    }
   

    // Start the game loop here
    // 在这里开始游戏主体的循环
        while (1) {
            printBoard(&gameState);
            printf("Player %s's turn. Enter your move (e.g., H8 or H12): ",
                (gameState.currentPlayer == PLAYER_BLACK) ? "Black (●)" : "White (◎)");
            int row, col;
            // Read a token safely and check return value
            //当前为PVP模式或者轮到人类下棋时，可以输入落子
            if (mode ==1 ||AIturn ==0) {
                if (scanf("%99s", input) != 1) {
                    printf("Input error. Exiting.\n");
                    break;
                }
            
            // Parse column letter (支持小写)
            char colChar = input[0];
            if (colChar >= 'a' && colChar <= 'z') colChar = (char)(colChar - ('a' - 'A'));
            col = colChar - 'A';

            // Parse row number (支持多位数字，如 12)
            char* rowStr = &input[1];
            char* endptr = NULL;
            long rowNum = strtol(rowStr, &endptr, 10);

            // Validate parsing and ranges
            if (input[0] == '\0' || endptr == rowStr || *endptr != '\0' ||
                col < 0 || col >= BOARD_SIZE ||
                rowNum < 1 || rowNum > BOARD_SIZE) {
                printf("Invalid move format or out of bounds. Use letter+number, e.g., H8 or H12.\n");
                continue;
            }
            AIturn = 1;
            row = BOARD_SIZE - (int)rowNum; // convert to internal row index
        }
            else {
                Position AIstep(GameState * gameState);
				row = AIstep(&gameState).row;
                col = AIstep(&gameState).col;
				printf("AI plays at %c%d\n", 'A' + col, BOARD_SIZE - row);
                AIturn = 0;
            }
        if (isValidMove(&gameState, row, col)) {
            // 先落子
            CellState stone = (gameState.currentPlayer == PLAYER_BLACK) ? BLACK : WHITE;
            gameState.board[row][col] = stone;

            // === 黑棋禁手判断：禁手就撤销并重下 ===
            if (stone == BLACK) {
                if (isForbiddenBlackMove(&gameState, row, col)) {
                    gameState.board[row][col] = EMPTY; // 撤销
					AIturn = 1 - AIturn; // 轮次不变
                    printf("Forbidden move for Black (禁手)! Try again.\n");
                    continue;
                }
            }
            // === 禁手判断结束 ===
            gameState.lastMove.row = row;
            gameState.lastMove.col = col;

            int winStatus = checkWin(&gameState, row, col);
            if (winStatus == 1) {
                printBoard(&gameState);
                printf("Black (●) wins!\n");
                break;
            }
            else if (winStatus == 2) {
                printBoard(&gameState);
                printf("White (◎) wins!\n");
                break;
            }

            // Switch player
            gameState.currentPlayer = (gameState.currentPlayer == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
        }

        else {
            printf("Invalid move. Try again.\n");
            AIturn = 1 - AIturn; // 轮次不变
        }
    }

    return 0;
}
