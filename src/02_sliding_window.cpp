// ============================================================
// 专题 02：滑动窗口
//
// 适用题型：最长无重复字符子串、最小覆盖子串、长度最小的子数组、
//           定长窗口最大值、字符串的排列。
//
// 核心思想：
//   维护一个窗口 [left, right]，right 负责向右扩张，left 负责收缩，
//   窗口内始终保存"当前正在考虑的一段连续区间"。
//   因为 left 和 right 都只向右移动，所以整个过程是 O(n)。
//
// 两种模板只在"记录答案的时机"上不同：
//   - 最长型：while 循环把窗口收缩到合法后，在 while 外面更新答案；
//   - 最短型：while 循环里窗口还合法时就更新答案，然后再收缩。
//
// 时间复杂度：O(n)（两个指针各走一遍数组）
// 空间复杂度：O(字符集大小)，本题用固定长度数组，可视为 O(1)
//
// 易错点：
//   1. 收缩窗口用 while 而不是 if —— 一次可能要缩很多步。
//   2. 最长型在 while 外记录答案，最短型在 while 内记录，写反就错。
//   3. 移动 left 时要同步"回滚"窗口状态（计数、总和等），
//      只移动指针不改状态是最常见的 bug。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/02_sliding_window.cpp -o /tmp/demo02
//   /tmp/demo02
// ============================================================

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

// ------------------------------------------------------------
// 题目 1：无重复字符的最长子串（LeetCode 3）—— 最长型窗口
//
// 思路：
//   right 每次把一个新字符纳入窗口，用 count 记录每个字符出现次数。
//   如果新字符让窗口出现重复（count > 1），就让 left 右移，
//   直到这个重复字符只剩一个。此时窗口一定合法，更新答案。
//
// 时间复杂度 O(n)，空间复杂度 O(字符集大小)。
// ------------------------------------------------------------
int lengthOfLongestSubstring(const string& text) {
    vector<int> count(128, 0);  // ASCII 字符的出现次数
    int left = 0;
    int best = 0;

    for (int right = 0; right < (int)text.size(); ++right) {
        char current = text[right];
        ++count[current];

        // 窗口不合法：有字符出现了两次，收缩左边界
        while (count[current] > 1) {
            --count[text[left]];
            ++left;
        }

        // 此时窗口 [left, right] 内没有重复字符
        best = std::max(best, right - left + 1);
    }
    return best;
}

// ------------------------------------------------------------
// 题目 2：长度最小的子数组（LeetCode 209）—— 最短型窗口
//
// 思路：
//   窗口内是连续的一段，维护窗口和 sum。
//   只要 sum >= target 就说明当前窗口可行，先记录长度，
//   再尝试把 left 右移，看能不能得到更短的合法窗口。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 返回值：不存在时返回 0。
// ------------------------------------------------------------
int minSubArrayLen(int target, const vector<int>& numbers) {
    int left = 0;
    int sum = 0;
    int best = INT_MAX;

    for (int right = 0; right < (int)numbers.size(); ++right) {
        sum += numbers[right];

        // 窗口合法：先记录答案（此时还合法），再收缩
        while (sum >= target) {
            best = std::min(best, right - left + 1);
            sum -= numbers[left];
            ++left;
        }
    }
    return best == INT_MAX ? 0 : best;
}

// ------------------------------------------------------------
// 题目 3：最小覆盖子串（LeetCode 76）—— 最短型窗口 + 计数
//
// 思路：
//   1. need 记录目标串里每个字符还差多少个；
//   2. right 扩张，遇到需要的字符就减少 need；
//   3. 当 need 里所有计数都 <= 0，说明窗口已经覆盖目标串，
//      记录答案并尝试收缩 left。
//
// 为了不每次遍历 26 个字母判断"是否覆盖"，用 missing 记录
// 还需要凑齐的字符总数：missing == 0 就表示完全覆盖。
//
// 时间复杂度 O(n + m)，空间复杂度 O(字符集大小)。
// ------------------------------------------------------------
string minWindow(const string& source, const string& target) {
    if (target.empty()) return "";

    unordered_map<char, int> need;  // 字符 -> 还需要几个
    for (char c : target) ++need[c];
    int missing = (int)target.size();  // 还差多少个字符没凑齐

    int left = 0;
    int bestStart = 0;
    int bestLength = INT_MAX;

    for (int right = 0; right < (int)source.size(); ++right) {
        char current = source[right];
        if (need[current] > 0) {
            --missing;  // 这个字符正是我们需要的
        }
        --need[current];

        // 已经完全覆盖目标串：记录答案，并尝试收缩
        while (missing == 0) {
            if (right - left + 1 < bestLength) {
                bestLength = right - left + 1;
                bestStart = left;
            }
            char leaving = source[left];
            ++need[leaving];
            if (need[leaving] > 0) {
                ++missing;  // 移走的字符是必需的，窗口重新变得不完整
            }
            ++left;
        }
    }
    if (bestLength == INT_MAX) return "";
    return source.substr(bestStart, bestLength);
}

int main() {
    // 示例 1：最长无重复字符子串
    string text1 = "abcabcbb";
    int answer1 = lengthOfLongestSubstring(text1);
    cout << "最长无重复子串：s = \"" << text1 << "\"，答案 = " << answer1 << endl;
    assert(answer1 == 3);  // "abc"

    string text2 = "pwwkew";
    cout << "最长无重复子串：s = \"" << text2 << "\"，答案 = "
         << lengthOfLongestSubstring(text2) << endl;
    assert(lengthOfLongestSubstring(text2) == 3);  // "wke"

    // 示例 2：长度最小的子数组
    vector<int> numbers = {2, 3, 1, 2, 4, 3};
    int answer2 = minSubArrayLen(7, numbers);
    cout << "长度最小子数组：target = 7，答案 = " << answer2 << endl;
    assert(answer2 == 2);  // [4, 3]

    // 示例 3：最小覆盖子串
    string answer3 = minWindow("ADOBECODEBANC", "ABC");
    cout << "最小覆盖子串：s = \"ADOBECODEBANC\"，t = \"ABC\"，答案 = \""
         << answer3 << "\"" << endl;
    assert(answer3 == "BANC");

    cout << "02 滑动窗口：全部示例通过" << endl;
    return 0;
}
