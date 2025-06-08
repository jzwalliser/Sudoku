#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>
using namespace std;


class Sudoku {
private:
    vector<vector<int>> board;
    vector<vector<int>> solution;
    vector<vector<bool>> fixed;
    bool solvedCorrectly;

    bool isValid(int row, int col, int num) {
        if (num == 0) return true; // 空位置总是有效的
        
        // 检查行
        for (int j = 0; j < 9; j++) {
            if (j == col) continue;
            if (board[row][j] == num) {
                return false;
            }
        }

        // 检查列
        for (int i = 0; i < 9; i++) {
            if (i == row) continue;
            if (board[i][col] == num) {
                return false;
            }
        }

        // 检查3x3宫格
        int startRow = row - row % 3;
        int startCol = col - col % 3;
        for (int i = startRow; i < startRow + 3; i++) {
            for (int j = startCol; j < startCol + 3; j++) {
                if (i == row && j == col) continue;
                if (board[i][j] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    bool solveSudoku() {
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                if (board[row][col] == 0) {
                    for (int num = 1; num <= 9; num++) {
                        if (isValid(row, col, num)) {
                            board[row][col] = num;
                            if (solveSudoku()) {
                                return true;
                            }
                            board[row][col] = 0;
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    void generateCompleteBoard() {
        // 清空棋盘
        board = vector<vector<int>>(9, vector<int>(9, 0));
        
        // 使用更随机的生成方式
        vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        random_device rd;
        mt19937 g(rd());
        
        // 随机填充对角线上的3个宫格（互不影响）
        for (int box = 0; box < 3; box++) {
            shuffle(numbers.begin(), numbers.end(), g);
            int index = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int row = box * 3 + i;
                    int col = box * 3 + j;
                    board[row][col] = numbers[index++];
                }
            }
        }
        
        // 解剩余部分
        solveSudoku();
    }

    void digHoles(int holes) {
        vector<pair<int, int>> positions;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                positions.push_back({i, j});
            }
        }
        
        // 使用现代C++的shuffle替代random_shuffle
        random_device rd;
        mt19937 g(rd());
        shuffle(positions.begin(), positions.end(), g);
        
        int holesDug = 0;
        for (auto& pos : positions) {
            if (holesDug >= holes) break;
            
            int row = pos.first;
            int col = pos.second;
            if (board[row][col] != 0) {
                int backup = board[row][col];
                board[row][col] = 0;
                
                // 检查是否有唯一解
                vector<vector<int>> temp = board;
                if (!hasUniqueSolution()) {
                    board[row][col] = backup;
                } else {
                    fixed[row][col] = false;
                    holesDug++;
                }
            }
        }
    }

    bool hasUniqueSolution() {
        vector<vector<int>> temp = board;
        int solutions = 0;
        countSolutions(0, 0, solutions);
        board = temp;
        return solutions == 1;
    }

    void countSolutions(int row, int col, int& solutions) {
        if (solutions > 1) return;

        if (row == 9) {
            solutions++;
            return;
        }

        if (col == 9) {
            countSolutions(row + 1, 0, solutions);
            return;
        }

        if (board[row][col] != 0) {
            countSolutions(row, col + 1, solutions);
            return;
        }

        for (int num = 1; num <= 9; num++) {
            if (isValid(row, col, num)) {
                board[row][col] = num;
                countSolutions(row, col + 1, solutions);
                board[row][col] = 0;
                if (solutions > 1) return;
            }
        }
    }
    
    // 检查整个棋盘是否有效
    bool isBoardValid() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j] != 0 && !isValid(i, j, board[i][j])) {
                    return false;
                }
            }
        }
        return true;
    }

public:
    Sudoku(int holes) : board(9, vector<int>(9, 0)), 
               solution(9, vector<int>(9, 0)),
               fixed(9, vector<bool>(9, false)),
               solvedCorrectly(false) {
        // 生成随机数独
        generateCompleteBoard();
        solution = board;
        
        // 标记所有位置为固定（挖洞前）
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                fixed[i][j] = true;
            }
        }
        
        // 挖洞
        digHoles(holes);
        
        // 保存初始棋盘状态
        initial = board;
    }
    
    vector<vector<int>> initial;

    void print() const {
        // 打印列坐标
        cout << "   ";
        for (int col = 1; col <= 9; col++) {
            cout << " " << col;
            if(col % 3 == 0){
                cout << "  ";
            }
        }
        cout << endl;

        // 打印顶部边框
        cout << "  +-------+-------+-------+" << endl;

        for (int i = 0; i < 9; i++) {
            // 打印行坐标
            cout << i + 1 << " | ";

            for (int j = 0; j < 9; j++) {
                if (board[i][j] == 0) {
                    cout << " ";
                } else {
                    if(fixed[i][j]){
                        // 红色显示初始题目
                        printf("\033[1;31m%d\033[0m", board[i][j]);
                    } else {
                        // 绿色显示玩家填入的数字
                        printf("\033[1;32m%d\033[0m", board[i][j]);
                    }
                }

                if (j % 3 == 2) {
                    cout << " | ";
                } else {
                    cout << " ";
                }
            }

            cout << endl;

            // 打印分隔线
            if (i % 3 == 2) {
                cout << "  +-------+-------+-------+" << endl;
            }
        }
    }

    int insert(int row, int col, int num) {
        // 转换为0-based索引
        row--;
        col--;

        // 检查边界
        if (row < 0 || row >= 9 || col < 0 || col >= 9) {
            return 1; // 出界
        }

        // 检查固定位置
        if (fixed[row][col]) {
            return 2; // 无效位置（固定位置）
        }

        // 检查数字有效性
        if (num < 0 || num > 9) {
            return 3; // 无效数字
        }
        
        // 保存旧值
        int oldValue = board[row][col];
        board[row][col] = num;
        
        // 删除操作总是成功
        if (num == 0) {
            return 5; // 成功删除
        }
        
        // 检查冲突
        if (!isValid(row, col, num)) {
            
            return 4; // 冲突
        }

        return 5; // 成功插入
    }

    // 检查游戏是否完成（填满且正确）
    bool isComplete() {
        // 首先检查是否填满
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j] == 0) {
                    solvedCorrectly = false;
                    return false;
                }
            }
        }
        
        // 然后检查是否正确
        solvedCorrectly = isBoardValid();
        return solvedCorrectly;
    }
    
    // 检查是否已正确解决
    bool isSolvedCorrectly() const {
        return solvedCorrectly;
    }
};

int main() {
    cout << "\033[1;36m" << "========================================" << endl;
    cout << "        欢迎来到数独游戏！" << endl;
    cout << "========================================" << "\033[0m" << endl;
    
    while(true) {
        string level;
        cout << "\n请选择难度：" << endl;
        //cout << "0) 测试（仅挖1个空）" << endl;
        cout << "1) 简单（挖10个空）" << endl;
        cout << "2) 中档（挖25个空）" << endl;
        cout << "3) 困难（挖40个空）" << endl;
        cout << "4) 大师（挖55个空）" << endl;
        cout << "请输入序号：";
        cin >> level;
        
        int leveldict[] = {1, 10, 25, 40, 55};
        int holes;
        if(level[0] >= '0' && level[0] <= '4') {
            holes = leveldict[level[0] - '0'];
            string difficulty[] = {"测试", "简单", "中档", "困难", "大师"};
            cout << "\n\033[1;33m" << "已选择【" << difficulty[level[0]-'0'] 
                 << "】难度，挖空" << holes << "个位置\033[0m" << endl;
        } else {
            cout << "\n\033[1;33m您选择了一个奇怪的难度，现在调整为中档难度。\033[0m" << endl;
            holes = 25;
        }
        
        cout << "\n\033[1;37m输入格式：行 列 数字（例如：2 5 7）" << endl;
        cout << "输入0可以删除数字（例如：2 5 0）" << endl;
        cout << "红色数字是初始题目，绿色数字是您填入的答案\033[0m" << endl;
        cout << "\n\033[1;35m游戏开始！\033[0m" << endl;
        
        Sudoku game(holes);
        bool quitGame = false;
        
        while (!quitGame) {
            game.print();
    
            int row, col, num;
            cout << "\n请输入坐标和数字（空格分隔）：";
            cin >> row >> col >> num;
            if (cin.fail()) {
        		cin.clear();
        		cin.ignore();
        		cout << "\033[1;31m无效输入。\033[0m\n";
        		continue;
        		//cin.sync();
        		//cin.ignore();
        	}
            // 检查是否请求退出
            if (row == 0 || col == 0) {
                cout << "放弃当前游戏？(y/n): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    break;
                }
                continue;
            }
            
            // 处理输入
            
            int result = game.insert(row, col, num);
            switch(result) {
                case 1: 
                    cout << "\033[1;31m错误：坐标出界！请使用1-9范围内的坐标。\033[0m" << endl;
                    break;
                case 2: 
                    cout << "\033[1;31m错误：不能修改初始题目！\033[0m" << endl;
                    break;
                case 3: 
                    cout << "\033[1;31m错误：无效数字！请使用0-9范围内的数字。\033[0m" << endl;
                    break;
                case 4: 
                    cout << "\033[1;31m错误：数字冲突！请检查行列和宫格。\033[0m" << endl;
                    break;
            }
            
            // 检查游戏是否完成
            if (game.isComplete()) {
                if (game.isSolvedCorrectly()) {
                    cout << "\n\033[1;32m" << "========================================" << endl;
                    cout << "  恭喜！你正确完成了数独游戏！" << endl;
                    cout << "========================================" << "\033[0m" << endl;
                    game.print();
                    break;
                } else {
                    cout << "\n\033[1;31m" << "========================================" << endl;
                    cout << "  数独已填满，但存在错误！" << endl;
                    cout << "========================================" << "\033[0m" << endl;
                }
            }
        }
    
        char again;
        while(true) {
            cout << "\n\033[1;36m是否再来一局？（Y/N）\033[0m ";
            cin >> again;
        
            if(again == 'N' || again == 'n') {
                cout << "\n\033[1;35m" << "========================================" << endl;
                cout << "  感谢游玩！再见！" << endl;
                cout << "========================================" << "\033[0m" << endl;
                return 0;
            } else if(again == 'Y' || again == 'y') {
                break;
            } else {
                cout << "对不起，我没看懂什么意思。请回答Y或N。" << endl;
            }
        }
    }
}