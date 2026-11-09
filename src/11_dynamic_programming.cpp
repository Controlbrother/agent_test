// ============================================================
// 专题 11：动态规划（DP）
//
// 适用题型：爬楼梯、最大子数组和、打家劫舍、01 背包、完全背包、
//           最长递增子序列（LIS）、最长公共子序列（LCS）、
//           编辑距离、最长回文子串、最小路径和。
//
// 核心思想（三步法）：
//   1. 状态：dp[i] / dp[i][j] 到底表示什么？必须能用一句话说完整；
//   2. 转移：dp[i] 从哪些更小的状态推出来？
//   3. 边界 + 遍历顺序：dp[0] 是多少？从小到大还是从大到小？
//
//   面试里 80% 的 DP 卡在"状态定义说不清楚"，不是写不出代码。
//
// 时间复杂度 / 空间复杂度：每个函数单独标注。
//
// 易错点：
//   1. 01 背包内层必须倒序（保证每件物品只用一次），正序就变成完全背包。
//   2. 区间 DP 要按"区间长度"从小到大枚举，不能按左端点。
//   3. 双串 DP 的 dp 数组多开一行一列表示空串，注意下标错位（s[i-1]）。
//   4. 初始化时要想清楚"不合法状态"用什么值，比如求最小值用 INT_MAX。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/11_dynamic_programming.cpp -o /tmp/demo11
//   /tmp/demo11
// ============================================================

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ------------------------------------------------------------
// 1. 爬楼梯（LeetCode 70）
//
// 状态：dp[i] = 爬到第 i 阶有多少种方法
// 转移：dp[i] = dp[i-1] + dp[i-2]（最后一步要么跨 1 阶，要么跨 2 阶）
// 边界：dp[0] = 1（站着不动算一种），dp[1] = 1
// 优化：只依赖前两项，所以用两个变量滚动即可。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
long long climbStairs(int n) {
    if (n <= 2) return n;
    long long previousTwo = 1;  // dp[1]
    long long previousOne = 2;  // dp[2]
    for (int i = 3; i <= n; ++i) {
        long long current = previousOne + previousTwo;
        previousTwo = previousOne;
        previousOne = current;
    }
    return previousOne;
}

// ------------------------------------------------------------
// 2. 最大子数组和（LeetCode 53）—— 线性 DP
//
// 状态：dp[i] = 以 i 结尾的连续子数组的最大和
// 转移：dp[i] = max(dp[i-1] + nums[i], nums[i])
//              （接上前面，或者从自己重新开始）
// 答案：所有 dp[i] 里的最大值。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 边界：空数组返回 0（和 robHouses、lengthOfLIS 等函数保持一致）。
// ------------------------------------------------------------
int maxSubArraySum(const vector<int>& numbers) {
    if (numbers.empty()) return 0;

    int best = numbers[0];
    int current = numbers[0];  // dp[i]
    for (int i = 1; i < (int)numbers.size(); ++i) {
        current = std::max(numbers[i], current + numbers[i]);
        best = std::max(best, current);
    }
    return best;
}

// ------------------------------------------------------------
// 3. 打家劫舍（LeetCode 198）
//
// 状态：dp[i] = 考虑前 i 间房能偷到的最大金额
// 转移：dp[i] = max(dp[i-1], dp[i-2] + nums[i-1])
//              （不偷第 i 间，或者偷第 i 间 + 前 i-2 间的最优解）
// 边界：dp[0] = 0（没有房子），dp[1] = nums[0]
//
// 时间复杂度 O(n)，空间复杂度 O(1)（只用两个变量滚动）。
// ------------------------------------------------------------
int robHouses(const vector<int>& money) {
    if (money.empty()) return 0;
    int previousTwo = 0;         // dp[i-2]
    int previousOne = money[0];  // dp[i-1]
    for (int i = 1; i < (int)money.size(); ++i) {
        int current = std::max(previousOne, previousTwo + money[i]);
        previousTwo = previousOne;
        previousOne = current;
    }
    return previousOne;
}

// ------------------------------------------------------------
// 4. 01 背包：每件物品最多选一次
//
// 状态：dp[j] = 容量为 j 时能装下的最大价值
// 转移：dp[j] = max(dp[j], dp[j - weight[i]] + value[i])
// 顺序：内层容量必须"从大到小"倒序 —— 这样 dp[j - weight] 还是
//       "没考虑当前物品"的旧值，保证每件物品只用一次。
//
// 时间复杂度 O(物品数 × 容量)，空间复杂度 O(容量)。
// ------------------------------------------------------------
int knapsack01(int capacity, const vector<int>& weights,
               const vector<int>& values) {
    vector<int> dp(capacity + 1, 0);
    for (int i = 0; i < (int)weights.size(); ++i) {
        for (int j = capacity; j >= weights[i]; --j) {  // 倒序！
            dp[j] = std::max(dp[j], dp[j - weights[i]] + values[i]);
        }
    }
    return dp[capacity];
}

// ------------------------------------------------------------
// 5. 完全背包：每件物品可以选无限次
//
// 和 01 背包唯一区别：内层容量"从小到大"正序，
// 这样 dp[j - weight] 已经用上了当前物品，等于允许重复选。
//
// 时间复杂度 O(物品数 × 容量)，空间复杂度 O(容量)。
// ------------------------------------------------------------
int knapsackFull(int capacity, const vector<int>& weights,
                 const vector<int>& values) {
    vector<int> dp(capacity + 1, 0);
    for (int i = 0; i < (int)weights.size(); ++i) {
        for (int j = weights[i]; j <= capacity; ++j) {  // 正序！
            dp[j] = std::max(dp[j], dp[j - weights[i]] + values[i]);
        }
    }
    return dp[capacity];
}

// ------------------------------------------------------------
// 6. 零钱兑换（LeetCode 322）—— 完全背包的"最少个数"版本
//
// 状态：dp[j] = 凑出金额 j 需要的最少硬币数
// 转移：dp[j] = min(dp[j], dp[j - coin] + 1)
// 边界：dp[0] = 0，其他初始化为"不可达"（一个大数）
//
// 时间复杂度 O(硬币种类 × 金额)，空间复杂度 O(金额)。
// ------------------------------------------------------------
int coinChange(const vector<int>& coins, int amount) {
    const int UNREACHABLE = amount + 1;  // 硬币数不可能超过 amount + 1
    vector<int> dp(amount + 1, UNREACHABLE);
    dp[0] = 0;

    for (int coin : coins) {
        for (int j = coin; j <= amount; ++j) {
            dp[j] = std::min(dp[j], dp[j - coin] + 1);
        }
    }
    return dp[amount] == UNREACHABLE ? -1 : dp[amount];
}

// ------------------------------------------------------------
// 7. 最长递增子序列 LIS（LeetCode 300）—— O(n^2) 版本
//
// 状态：dp[i] = 以 nums[i] 结尾的最长递增子序列长度
// 转移：dp[i] = max(dp[j]) + 1，其中 j < i 且 nums[j] < nums[i]
//
// 时间复杂度 O(n^2)，空间复杂度 O(n)。
// （O(n log n) 的贪心 + 二分版本见 12_greedy.cpp）
// ------------------------------------------------------------
int lengthOfLIS(const vector<int>& numbers) {
    int n = (int)numbers.size();
    if (n == 0) return 0;

    vector<int> dp(n, 1);  // 每个数自己至少构成长度 1
    int best = 1;
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (numbers[j] < numbers[i]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
        }
        best = std::max(best, dp[i]);
    }
    return best;
}

// ------------------------------------------------------------
// 8. 最长公共子序列 LCS（LeetCode 1143）
//
// 状态：dp[i][j] = text1 前 i 个字符 和 text2 前 j 个字符 的 LCS 长度
// 转移：字符相同  -> dp[i][j] = dp[i-1][j-1] + 1
//       字符不同  -> dp[i][j] = max(dp[i-1][j], dp[i][j-1])
// 边界：dp[0][j] = dp[i][0] = 0（有一边是空串）
//
// 下标：dp 多开一行一列，dp[i][j] 对应字符 text1[i-1]、text2[j-1]。
//
// 时间复杂度 O(n*m)，空间复杂度 O(n*m)。
// ------------------------------------------------------------
int longestCommonSubsequence(const string& text1, const string& text2) {
    int n = (int)text1.size();
    int m = (int)text2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[n][m];
}

// ------------------------------------------------------------
// 9. 编辑距离（LeetCode 72）
//
// 状态：dp[i][j] = 把 word1 前 i 个字符变成 word2 前 j 个字符的最少操作数
// 转移：
//   字符相同 -> dp[i][j] = dp[i-1][j-1]
//   字符不同 -> dp[i][j] = 1 + min(
//                   dp[i-1][j],    // 删除
//                   dp[i][j-1],    // 插入
//                   dp[i-1][j-1])  // 替换
// 边界：dp[i][0] = i（全删），dp[0][j] = j（全插）
//
// 时间复杂度 O(n*m)，空间复杂度 O(n*m)。
// ------------------------------------------------------------
int editDistance(const string& word1, const string& word2) {
    int n = (int)word1.size();
    int m = (int)word2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    for (int i = 0; i <= n; ++i) dp[i][0] = i;
    for (int j = 0; j <= m; ++j) dp[0][j] = j;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({dp[i - 1][j],      // 删除
                                         dp[i][j - 1],      // 插入
                                         dp[i - 1][j - 1]});  // 替换
            }
        }
    }
    return dp[n][m];
}

// ------------------------------------------------------------
// 10. 最长回文子串（LeetCode 5）—— 区间 DP
//
// 状态：dp[i][j] = 子串 s[i..j] 是否是回文串
// 转移：s[i] == s[j] 且 s[i+1..j-1] 是回文 -> dp[i][j] = true
//       长度为 2 时，只要 s[i] == s[j] 就是回文（中间没有字符）
// 遍历顺序：必须按"区间长度"从小到大枚举，
//          因为 dp[i][j] 依赖 dp[i+1][j-1]（更短的区间）。
//
// 时间复杂度 O(n^2)，空间复杂度 O(n^2)。
// ------------------------------------------------------------
string longestPalindrome(const string& text) {
    int n = (int)text.size();
    if (n < 2) return text;

    vector<vector<bool>> dp(n, vector<bool>(n, false));
    int bestStart = 0;
    int bestLength = 1;

    for (int i = 0; i < n; ++i) dp[i][i] = true;  // 单个字符是回文

    for (int length = 2; length <= n; ++length) {
        for (int left = 0; left + length - 1 < n; ++left) {
            int right = left + length - 1;
            if (text[left] != text[right]) continue;

            if (length == 2) {
                dp[left][right] = true;
            } else {
                dp[left][right] = dp[left + 1][right - 1];
            }

            if (dp[left][right] && length > bestLength) {
                bestLength = length;
                bestStart = left;
            }
        }
    }
    return text.substr(bestStart, bestLength);
}

// ------------------------------------------------------------
// 11. 最小路径和（LeetCode 64）—— 二维路径 DP
//
// 状态：dp[i][j] = 从左上角走到 (i, j) 的最小路径和
// 转移：dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1])
// 边界：第一行只能从左边来，第一列只能从上边来。
//
// 时间复杂度 O(m*n)，空间复杂度 O(m*n)。
// ------------------------------------------------------------
int minPathSum(const vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;

    int rows = (int)grid.size();
    int cols = (int)grid[0].size();
    vector<vector<long long>> dp(rows, vector<long long>(cols, 0));

    dp[0][0] = grid[0][0];
    for (int i = 1; i < rows; ++i) dp[i][0] = dp[i - 1][0] + grid[i][0];
    for (int j = 1; j < cols; ++j) dp[0][j] = dp[0][j - 1] + grid[0][j];

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {
            dp[i][j] = std::min(dp[i - 1][j], dp[i][j - 1]) + grid[i][j];
        }
    }
    return (int)dp[rows - 1][cols - 1];
}

int main() {
    // 示例 1：爬楼梯
    cout << "爬楼梯：n = 5 -> " << climbStairs(5) << " 种" << endl;
    assert(climbStairs(5) == 8);

    // 示例 2：最大子数组和
    vector<int> numbers = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int maxSum = maxSubArraySum(numbers);
    cout << "最大子数组和：[-2, 1, -3, 4, -1, 2, 1, -5, 4] -> " << maxSum << endl;
    assert(maxSum == 6);  // [4, -1, 2, 1]

    // 边界：空数组不能崩（各 DP 函数对空输入的处理要一致）
    assert(maxSubArraySum({}) == 0);
    assert(robHouses({}) == 0);
    assert(lengthOfLIS({}) == 0);
    assert(longestPalindrome("") == "");
    assert(minPathSum({}) == 0);

    // 示例 3：打家劫舍
    cout << "打家劫舍：[2, 7, 9, 3, 1] -> " << robHouses({2, 7, 9, 3, 1}) << endl;
    assert(robHouses({2, 7, 9, 3, 1}) == 12);  // 偷 2 + 9 + 1

    // 示例 4 / 5：01 背包与完全背包
    vector<int> weights = {1, 3, 4};
    vector<int> values = {15, 20, 30};
    int best01 = knapsack01(4, weights, values);
    int bestFull = knapsackFull(4, weights, values);
    cout << "背包容量 4，物品 (重量,价值) = (1,15) (3,20) (4,30)" << endl;
    cout << "  01 背包最大价值 = " << best01 << "（每件最多一次）" << endl;
    cout << "  完全背包最大价值 = " << bestFull << "（可重复选）" << endl;
    assert(best01 == 35);    // 1 + 3
    assert(bestFull == 60);  // 4 个重量 1 的物品

    // 示例 6：零钱兑换
    cout << "零钱兑换：coins = [1, 2, 5]，amount = 11 -> "
         << coinChange({1, 2, 5}, 11) << " 枚" << endl;
    assert(coinChange({1, 2, 5}, 11) == 3);   // 5 + 5 + 1
    assert(coinChange({2}, 3) == -1);          // 凑不出来

    // 示例 7：最长递增子序列
    cout << "LIS：[10, 9, 2, 5, 3, 7, 101, 18] -> "
         << lengthOfLIS({10, 9, 2, 5, 3, 7, 101, 18}) << endl;
    assert(lengthOfLIS({10, 9, 2, 5, 3, 7, 101, 18}) == 4);  // 2, 3, 7, 101

    // 示例 8：最长公共子序列
    cout << "LCS：\"abcde\" 与 \"ace\" -> "
         << longestCommonSubsequence("abcde", "ace") << endl;
    assert(longestCommonSubsequence("abcde", "ace") == 3);

    // 示例 9：编辑距离
    cout << "编辑距离：\"horse\" -> \"ros\" = "
         << editDistance("horse", "ros") << " 次操作" << endl;
    assert(editDistance("horse", "ros") == 3);

    // 示例 10：最长回文子串
    cout << "最长回文子串：\"babad\" -> \"" << longestPalindrome("babad") << "\""
         << "，\"cbbd\" -> \"" << longestPalindrome("cbbd") << "\"" << endl;
    assert(longestPalindrome("babad") == "bab" ||
           longestPalindrome("babad") == "aba");
    assert(longestPalindrome("cbbd") == "bb");
    assert(longestPalindrome("a") == "a");

    // 示例 11：最小路径和
    vector<vector<int>> grid = {
        {1, 3, 1},
        {1, 5, 1},
        {4, 2, 1},
    };
    cout << "最小路径和：3x3 网格 -> " << minPathSum(grid) << endl;
    assert(minPathSum(grid) == 7);  // 1 -> 3 -> 1 -> 1 -> 1

    cout << "11 动态规划：全部示例通过" << endl;
    return 0;
}
