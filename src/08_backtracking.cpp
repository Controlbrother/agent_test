// ============================================================
// 专题 08：回溯（DFS）
//
// 适用题型：子集、组合、全排列（含重复元素）、组合总和、
//           括号生成、N 皇后、单词搜索。
//
// 核心思想：
//   回溯 = 决策树的深度优先遍历。固定三段式：
//     1. 做选择：把当前选项加入路径；
//     2. 递归：进入下一层继续选；
//     3. 撤销选择：把路径恢复原状，再试下一个选项。
//   写不出回溯，通常是因为没有把决策树画清楚。
//
// 三个经典变体的区别：
//   - 子集：用 start 控制"只能往后选"，每个节点都收集一次答案；
//   - 组合：用 start 控制，只在 path.size() == k 时收集；
//   - 排列：用 used 标记，每次从头遍历，path.size() == n 时收集。
//
// 时间复杂度：子集 / 组合 O(2^n * n)，排列 O(n! * n)（复制路径的开销）
// 空间复杂度：O(n)（递归深度 + path）
//
// 易错点：
//   1. 忘记撤销选择（path.pop_back()、used[i] = false），是最常见的 bug。
//   2. 含重复元素时，要先排序，再在同层跳过相同值。
//   3. 剪枝条件要和题目对应：组合总和的"和已超标"、括号生成的"右括号不能多"。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/08_backtracking.cpp -o /tmp/demo08
//   /tmp/demo08
// ============================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ------------------------------------------------------------
// 题目 1：子集（LeetCode 78）
//
// 思路：
//   每个元素都有"选 / 不选"两种可能。用 start 保证只向后选，
//   这样 [1,2] 和 [2,1] 不会被当成两个不同的子集。
//   每进入一层就把当前 path 收集一次（包括空集）。
//
// 时间复杂度 O(2^n * n)，空间复杂度 O(n)。
// ------------------------------------------------------------
void collectSubsets(const vector<int>& numbers, int start,
                    vector<int>& path, vector<vector<int>>& result) {
    result.push_back(path);  // 每个节点都是一个子集

    for (int i = start; i < (int)numbers.size(); ++i) {
        path.push_back(numbers[i]);          // 做选择
        collectSubsets(numbers, i + 1, path, result);  // 递归
        path.pop_back();                     // 撤销选择
    }
}

vector<vector<int>> subsets(const vector<int>& numbers) {
    vector<vector<int>> result;
    vector<int> path;
    collectSubsets(numbers, 0, path, result);
    return result;
}

// ------------------------------------------------------------
// 题目 2：组合（LeetCode 77）—— 从 1..n 里选 k 个
//
// 思路：
//   和子集一样用 start 控制，但只在 path 长度等于 k 时收集，
//   并且提前剪枝：剩下能选的数不够凑满 k 个时直接返回。
//
// 时间复杂度 O(C(n, k) * k)，空间复杂度 O(k)。
// ------------------------------------------------------------
void collectCombinations(int n, int k, int start,
                         vector<int>& path, vector<vector<int>>& result) {
    if ((int)path.size() == k) {
        result.push_back(path);
        return;
    }
    // 剪枝：从 start 到 n 一共剩 n - start + 1 个数，
    // 如果不够凑满 k 个，就没必要继续了
    if (n - start + 1 < k - (int)path.size()) {
        return;
    }

    for (int value = start; value <= n; ++value) {
        path.push_back(value);
        collectCombinations(n, k, value + 1, path, result);
        path.pop_back();
    }
}

vector<vector<int>> combine(int n, int k) {
    vector<vector<int>> result;
    vector<int> path;
    collectCombinations(n, k, 1, path, result);
    return result;
}

// ------------------------------------------------------------
// 题目 3：全排列（含重复元素，LeetCode 47）
//
// 思路：
//   排列要"每个位置都能放任何还没用过的数"，所以每次从 0 开始遍历，
//   用 used 标记哪些位置已经用过。长度凑满 n 就是一个答案。
//
//   去重：先排序，同一层里如果前一个相同数字刚被撤销（!used[i-1]），
//   说明这一层已经用这个值开过头了，跳过，否则会出现重复排列。
//
// 时间复杂度 O(n! * n)，空间复杂度 O(n)。
// ------------------------------------------------------------
void collectPermutations(const vector<int>& numbers, vector<bool>& used,
                         vector<int>& path, vector<vector<int>>& result) {
    if ((int)path.size() == (int)numbers.size()) {
        result.push_back(path);
        return;
    }

    for (int i = 0; i < (int)numbers.size(); ++i) {
        if (used[i]) continue;

        // 同层去重：相同数字在同一层只用第一个
        if (i > 0 && numbers[i] == numbers[i - 1] && !used[i - 1]) {
            continue;
        }

        used[i] = true;                    // 做选择
        path.push_back(numbers[i]);
        collectPermutations(numbers, used, path, result);
        path.pop_back();                   // 撤销选择
        used[i] = false;
    }
}

vector<vector<int>> permuteUnique(vector<int> numbers) {
    std::sort(numbers.begin(), numbers.end());  // 排序是去重的前提
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(numbers.size(), false);
    collectPermutations(numbers, used, path, result);
    return result;
}

// ------------------------------------------------------------
// 题目 4：组合总和（LeetCode 39，可重复选）
//
// 思路：
//   和组合类似，但每个数可以重复使用，所以递归时 start 传 i 而不是 i + 1。
//   剪枝：如果当前和已经超过 target，直接返回（数组已排序时还能 break）。
//
// 时间复杂度 O(答案个数 * 平均长度)，空间复杂度 O(target / 最小数)。
// ------------------------------------------------------------
void collectCombinationSum(const vector<int>& candidates, int target, int start,
                           int currentSum, vector<int>& path,
                           vector<vector<int>>& result) {
    if (currentSum == target) {
        result.push_back(path);
        return;
    }

    for (int i = start; i < (int)candidates.size(); ++i) {
        if (currentSum + candidates[i] > target) {
            break;  // 已排序，后面的更大，直接结束这一层
        }
        path.push_back(candidates[i]);
        collectCombinationSum(candidates, target, i,  // 传 i：允许重复选自己
                              currentSum + candidates[i], path, result);
        path.pop_back();
    }
}

vector<vector<int>> combinationSum(vector<int> candidates, int target) {
    std::sort(candidates.begin(), candidates.end());
    vector<vector<int>> result;
    vector<int> path;
    collectCombinationSum(candidates, target, 0, 0, path, result);
    return result;
}

// ------------------------------------------------------------
// 题目 5：括号生成（LeetCode 22）
//
// 思路：
//   一共 2n 个位置，每个位置放 '(' 或 ')'。合法条件是：
//   - 左括号最多 n 个；
//   - 任何时候右括号数量不能超过左括号。
//   用 open 和 close 记录已用的左右括号数，凑满 2n 就收集答案。
//
// 时间复杂度 O(第 n 个卡特兰数)，空间复杂度 O(n)。
// ------------------------------------------------------------
void generateParentheses(int n, int open, int close,
                         string& path, vector<string>& result) {
    if ((int)path.size() == 2 * n) {
        result.push_back(path);
        return;
    }
    if (open < n) {
        path.push_back('(');
        generateParentheses(n, open + 1, close, path, result);
        path.pop_back();
    }
    if (close < open) {  // 右括号不能比左括号多
        path.push_back(')');
        generateParentheses(n, open, close + 1, path, result);
        path.pop_back();
    }
}

vector<string> generateParenthesis(int n) {
    vector<string> result;
    string path;
    generateParentheses(n, 0, 0, path, result);
    return result;
}

// ------------------------------------------------------------
// 题目 6：N 皇后（LeetCode 51）—— 经典剪枝
//
// 思路：
//   一行一行放皇后。放第 row 行时，尝试每一列 col，
//   检查这一列、两条对角线上是否已经有皇后。
//   用三个数组快速判断：列、主对角线（row - col）、副对角线（row + col）。
//
// 时间复杂度 O(n!)，空间复杂度 O(n)。
// ------------------------------------------------------------
void solveQueens(int row, int n, vector<int>& columns,
                 vector<bool>& colUsed, vector<bool>& diagUsed,
                 vector<bool>& antiDiagUsed, vector<vector<string>>& result) {
    if (row == n) {
        // 把每行皇后所在的列号翻译成棋盘
        vector<string> board(n, string(n, '.'));
        for (int r = 0; r < n; ++r) {
            board[r][columns[r]] = 'Q';
        }
        result.push_back(board);
        return;
    }

    for (int col = 0; col < n; ++col) {
        int diagIndex = row - col + n - 1;  // 主对角线编号，加偏移避免负数
        int antiDiagIndex = row + col;      // 副对角线编号
        if (colUsed[col] || diagUsed[diagIndex] || antiDiagUsed[antiDiagIndex]) {
            continue;  // 这一列或两条对角线已经被占用
        }

        columns[row] = col;
        colUsed[col] = true;
        diagUsed[diagIndex] = true;
        antiDiagUsed[antiDiagIndex] = true;

        solveQueens(row + 1, n, columns, colUsed, diagUsed, antiDiagUsed, result);

        // 撤销选择
        colUsed[col] = false;
        diagUsed[diagIndex] = false;
        antiDiagUsed[antiDiagIndex] = false;
    }
}

vector<vector<string>> solveNQueens(int n) {
    vector<vector<string>> result;
    vector<int> columns(n, 0);
    vector<bool> colUsed(n, false);
    vector<bool> diagUsed(2 * n, false);
    vector<bool> antiDiagUsed(2 * n, false);
    solveQueens(0, n, columns, colUsed, diagUsed, antiDiagUsed, result);
    return result;
}

// ------------------------------------------------------------
// 题目 7：单词搜索（LeetCode 79）—— 网格 DFS + 回溯
//
// 思路：
//   从每个格子出发做 DFS，依次匹配单词的字符。
//   走过的格子先临时改成 '#'，防止重复走；回溯时改回来。
//   四个方向只要有一个能走通就返回 true。
//
// 时间复杂度 O(m * n * 4^L)，L 是单词长度；空间复杂度 O(L)。
// ------------------------------------------------------------
bool searchWord(vector<vector<char>>& board, const string& word, int row, int col,
                int index) {
    if (index == (int)word.size()) return true;  // 全部字符都匹配上了

    int rows = (int)board.size();
    int cols = (int)board[0].size();
    if (row < 0 || row >= rows || col < 0 || col >= cols) return false;
    if (board[row][col] != word[index]) return false;

    char saved = board[row][col];
    board[row][col] = '#';  // 标记已访问，防止走回头路

    bool found = searchWord(board, word, row + 1, col, index + 1) ||
                 searchWord(board, word, row - 1, col, index + 1) ||
                 searchWord(board, word, row, col + 1, index + 1) ||
                 searchWord(board, word, row, col - 1, index + 1);

    board[row][col] = saved;  // 回溯：恢复现场
    return found;
}

bool existWord(vector<vector<char>> board, const string& word) {
    if (board.empty() || board[0].empty()) return word.empty();
    for (int row = 0; row < (int)board.size(); ++row) {
        for (int col = 0; col < (int)board[0].size(); ++col) {
            if (searchWord(board, word, row, col, 0)) {
                return true;
            }
        }
    }
    return false;
}

// 打印二维结果（每个答案一行）
void printStringGroups(const vector<vector<string>>& groups) {
    for (const vector<string>& group : groups) {
        cout << "  ";
        for (const string& item : group) cout << item << " ";
        cout << endl;
    }
}

int main() {
    // 示例 1：子集
    vector<vector<int>> allSubsets = subsets({1, 2, 3});
    cout << "子集：共 " << allSubsets.size() << " 个（应为 8 个）" << endl;
    assert(allSubsets.size() == 8);

    // 示例 2：组合
    vector<vector<int>> allCombinations = combine(4, 2);
    cout << "组合 C(4, 2)：共 " << allCombinations.size() << " 个（应为 6 个）" << endl;
    assert(allCombinations.size() == 6);

    // 示例 3：含重复元素的全排列
    vector<vector<int>> allPermutations = permuteUnique({1, 1, 2});
    cout << "全排列 [1, 1, 2]：共 " << allPermutations.size()
         << " 个（应为 3 个）" << endl;
    assert(allPermutations.size() == 3);

    // 示例 4：组合总和
    vector<vector<int>> sumCombinations = combinationSum({2, 3, 6, 7}, 7);
    cout << "组合总和 target = 7：共 " << sumCombinations.size()
         << " 个（应为 2 个）" << endl;
    assert(sumCombinations.size() == 2);

    // 示例 5：括号生成
    vector<string> parentheses = generateParenthesis(3);
    cout << "括号生成 n = 3：" << endl;
    for (const string& item : parentheses) cout << "  " << item << endl;
    assert(parentheses.size() == 5);

    // 示例 6：N 皇后
    vector<vector<string>> queens = solveNQueens(4);
    cout << "N 皇后 n = 4：共 " << queens.size() << " 种解法（应为 2 种），第一种：" << endl;
    printStringGroups(queens);
    assert(queens.size() == 2);

    // 示例 7：单词搜索
    vector<vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'},
    };
    cout << "单词搜索：ABCCED -> " << (existWord(board, "ABCCED") ? "找到" : "没找到")
         << "，ABCB -> " << (existWord(board, "ABCB") ? "找到" : "没找到") << endl;
    assert(existWord(board, "ABCCED") == true);
    assert(existWord(board, "ABCB") == false);

    cout << "08 回溯：全部示例通过" << endl;
    return 0;
}
