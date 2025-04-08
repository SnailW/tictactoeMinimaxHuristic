#include <bits/stdc++.h>

using namespace std;

// 게임 설정
#define BOARDSIZE 7
#define WINCONDITION 5
#define DEPTHLIMIT 15
#define AI_PLAYER 'X'
#define HUMAN_PLAYER 'O'

// 보드 생성
char board[BOARDSIZE][BOARDSIZE];

// 보드 초기화
void boardReset() {
    for (int i = 0; i < BOARDSIZE; i++)
        fill(board[i], board[i] + BOARDSIZE, ' ');
}

// 보드가 가득 찼는지 확인
bool isBoardFull() {
    for (int i = 0; i < BOARDSIZE; i++)
        for (int j = 0; j < BOARDSIZE; j++)
            if (board[i][j] == ' ')
                return false;
    return true;
}

// 보드 출력
void printBoard() {
    cout << "\n";
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            cout << " " << board[i][j] << " ";
            if (j < BOARDSIZE - 1) cout << "|";
        }
        cout << '\n';
        if (i < BOARDSIZE - 1) {
            for (int j = 0; j < BOARDSIZE * 4 - 1; j++) cout << "-";
            cout << '\n';
        }
    }
    cout << '\n';
}

// 특정 방향으로 연속된 돌 개수를 세기
int countConsecutive(char player, int r, int c, int dr, int dc) {
    int count = 0;
    for (int i = 0; i < WINCONDITION; ++i) {
        int nr = r + i * dr;
        int nc = c + i * dc;
        if (nr >= 0 && nr < BOARDSIZE && nc >= 0 && nc < BOARDSIZE && board[nr][nc] == player) {
            count++;
        } else {
            break;
        }
    }
    return count;
}

// 승리 조건 확인 ( countConsecutive() 합치기 )
bool winChk(char player, pair<int, int> pos) {
    int r = pos.first;
    int c = pos.second;

    // 각 방향에 대해 연속된 돌의 개수를 확인
    auto count = [&](int dr, int dc) {
        int cnt = 0;
        for (int i = 1; ; ++i) {
            int nr = r + i * dr;
            int nc = c + i * dc;
            if (nr >= 0 && nr < BOARDSIZE && nc >= 0 && nc < BOARDSIZE && board[nr][nc] == player) {
                cnt++;
            } else {
                break;
            }
        }
        return cnt;
    };

    // 가로
    if (count(0, 1) + count(0, -1) + 1 >= WINCONDITION) return true;
    // 세로
    if (count(1, 0) + count(-1, 0) + 1 >= WINCONDITION) return true;
    // \ 대각선
    if (count(1, 1) + count(-1, -1) + 1 >= WINCONDITION) return true;
    // / 대각선
    if (count(1, -1) + count(-1, 1) + 1 >= WINCONDITION) return true;

    return false;
}

// 현재 게임 상태 평가 ( 추후 전체적인 평가 값 수정 필요 )
int evaluate(pair<int, int> lastMove) {
    if (lastMove.first == -1) return 0; // 초기 상태

    if (winChk(AI_PLAYER, lastMove)) return 1000;
    if (winChk(HUMAN_PLAYER, lastMove)) return -1000;
    if (isBoardFull()) return 0;
    return 0; // 게임이 끝나지 않았으면 0 (추후 평가 값 수정)
}

// 휴리스틱 평가 ( 추후 전체적인 수정 필요 )
int evaluateHeuristic() {
    int score = 0;
    // 각 방향에 대해 AI와 플레이어의 연속된 돌 개수를 세어 평가
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j] == AI_PLAYER) {
                score += countConsecutive(AI_PLAYER, i, j, 0, 1);
                score += countConsecutive(AI_PLAYER, i, j, 1, 0);
                score += countConsecutive(AI_PLAYER, i, j, 1, 1);
                score += countConsecutive(AI_PLAYER, i, j, 1, -1);
            } else if (board[i][j] == HUMAN_PLAYER) {
                score -= countConsecutive(HUMAN_PLAYER, i, j, 0, 1);
                score -= countConsecutive(HUMAN_PLAYER, i, j, 1, 0);
                score -= countConsecutive(HUMAN_PLAYER, i, j, 1, 1);
                score -= countConsecutive(HUMAN_PLAYER, i, j, 1, -1);
            }
        }
    }
    return score;
}

// Minimax (Alpha-Beta Pruning)
int minimax(int depth, int depthLimit, bool isMaximizing, pair<int, int> lastMove, int alpha, int beta) {
    int score = evaluate(lastMove);

    if (score != 0 || depth == depthLimit || isBoardFull()) {
        return score != 0 ? score : evaluateHeuristic();
    }

    if (isMaximizing) {
        int bestScore = -0x7f7f7f7f;
        for (int i = 0; i < BOARDSIZE; i++) {
            for (int j = 0; j < BOARDSIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = AI_PLAYER;
                    int currentScore = minimax(depth + 1, depthLimit, false, {i, j}, alpha, beta);
                    board[i][j] = ' ';
                    bestScore = max(bestScore, currentScore);
                    alpha = max(alpha, bestScore);
                    if (beta <= alpha) {
                        return bestScore;
                    }
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 0x7f7f7f7f;
        for (int i = 0; i < BOARDSIZE; i++) {
            for (int j = 0; j < BOARDSIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = HUMAN_PLAYER;
                    int currentScore = minimax(depth + 1, depthLimit, true, {i, j}, alpha, beta);
                    board[i][j] = ' ';
                    bestScore = min(bestScore, currentScore);
                    beta = min(beta, bestScore);
                    if (beta <= alpha) {
                        return bestScore;
                    }
                }
            }
        }
        return bestScore;
    }
}

// AI가 둘 최적의 수 찾기 (Iterative Deepening 및 시간 제한 추가)
pair<int, int> findBestMove(double timeLimitSeconds) {
    int bestScore = -0x7f7f7f7f;
    pair<int, int> bestMove = {-1, -1};
    int alpha = -0x7f7f7f7f;
    int beta = 0x7f7f7f7f;
    auto startTime = chrono::high_resolution_clock::now();
    int depthLimit = 1;

    // cout << "\nAI가 최적의 수를 찾는 중 (최대 시간: " << timeLimitSeconds << "초)...\n";

    while (true) {
        int currentBestScore = -0x7f7f7f7f;
        pair<int, int> currentBestMove = {-1, -1};

        vector<pair<int, int>> possibleMoves;
        for (int i = 0; i < BOARDSIZE; ++i) {
            for (int j = 0; j < BOARDSIZE; ++j) {
                if (board[i][j] == ' ') {
                    possibleMoves.push_back({i, j});
                }
            }
        }

        // 마지막 수 주변 먼저 탐색
        if (bestMove.first != -1) {
            sort(possibleMoves.begin(), possibleMoves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
                int distA = abs(a.first - bestMove.first) + abs(a.second - bestMove.second);
                int distB = abs(b.first - bestMove.first) + abs(b.second - bestMove.second);
                return distA < distB;
            });
        }

        for (const auto& move : possibleMoves) {
            board[move.first][move.second] = AI_PLAYER;
            int score = minimax(0, depthLimit, false, move, alpha, beta);
            board[move.first][move.second] = ' ';

            // cout << "    (깊이 " << depthLimit << ") 칸 (" << move.first << ", " << move.second << ") 점수: " << score << '\n';

            if (score > currentBestScore) {
                currentBestScore = score;
                currentBestMove = move;
            }
        }

        if (currentBestMove.first != -1) {
            bestScore = currentBestScore;
            bestMove = currentBestMove;
        }

        auto endTime = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = endTime - startTime;

        cout << "(깊이 " << depthLimit << ") 탐색 완료. 시간: " << duration.count() << "초, 최고 점수: " << bestScore << ", 최적 위치: (" << bestMove.first << ", " << bestMove.second << ")\n";

        if (duration.count() >= timeLimitSeconds || depthLimit >= DEPTHLIMIT) {
            cout << (duration.count() >= timeLimitSeconds ? "시간 초과" : "최대 깊이 도달") << ". AI가 선택한 위치: (" << bestMove.first << ", " << bestMove.second << ")\n";
            return bestMove;
        }

        depthLimit++;
    }
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    boardReset();
    printBoard();

    bool endFlag = false;
    char currentPlayer = HUMAN_PLAYER;
    pair<int, int> lastMove = {-1, -1};

    while (!endFlag) {
        pair<int, int> pos;

        if (currentPlayer == HUMAN_PLAYER) {
            while (true) {
                int row, col;
                cout << "'O' 놓을 위치 (행 열): ";
                cin >> row >> col;
                row--; col--;

                if (row >= 0 && row < BOARDSIZE && col >= 0 && col < BOARDSIZE && board[row][col] == ' ') {
                    pos = {row, col};
                    board[pos.first][pos.second] = HUMAN_PLAYER;
                    lastMove = pos;
                    break;
                } else if (row < 0 || row >= BOARDSIZE || col < 0 || col >= BOARDSIZE) {
                    cout << ">> 범위 오류! 0 ~ " << BOARDSIZE - 1 << " 사이의 행과 열 번호를 입력해주세요.\n";
                } else {
                    cout << ">> 이미 돌이 놓인 위치입니다. 다른 위치를 입력해주세요.\n";
                }
                cin.clear();
                cin.ignore(0x7f7f7f7f, '\n');
            }
            printBoard();

            if (winChk(HUMAN_PLAYER, pos)) {
                cout << "*** 축하합니다! O의 승리입니다! ***\n";
                endFlag = true;
            }
            currentPlayer = AI_PLAYER;
        } else {
            cout << "\n--- AI ('X')의 차례 ---\n";
            auto start = chrono::high_resolution_clock::now();
            pos = findBestMove(5.0); // 5초 시간 할당
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "AI의 이동 시간: " << duration.count() << " 초\n";

            if (pos.first == -1) {
                cout << "치명적 오류: AI가 둘 곳을 찾지 못했습니다!\n";
                endFlag = true;
                continue;
            }

            board[pos.first][pos.second] = AI_PLAYER;
            lastMove = pos;
            printBoard();

            if (winChk(AI_PLAYER, pos)) {
                cout << "*** AI ('X')가 승리했습니다! ***\n";
                endFlag = true;
            }
            currentPlayer = HUMAN_PLAYER;
        }

        if (!endFlag && isBoardFull()) {
            cout << "--- 무승부입니다! ---\n";
            endFlag = true;
        }
    }

    cout << "\n게임 종료.\n";
    return 0;
}