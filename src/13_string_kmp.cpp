// ============================================================
// 专题 13：字符串 / KMP
//
// 适用题型：KMP 字符串匹配、最长公共前缀、反转单词、
//           字符串相加（大数）、判断回文、字符统计。
//
// 核心思想：
//   KMP 要解决的是"在主串里找子串出现的位置"。
//   暴力做法每次失配都要回退到"这次匹配起点的下一个位置"，最坏 O(n*m)。
//   KMP 用 next 数组记住"失配后模式串应该退到哪一位"，
//   让主串指针永不回退，整体 O(n + m)。
//
//   next[i] 的含义：模式串的前 i+1 个字符中，
//   "既是前缀又是后缀"的最长长度（最长公共前后缀）。
//   例如 "abab" 的 next = [0, 0, 1, 2]。
//
// 时间复杂度 O(n + m)，空间复杂度 O(m)。
//
// 易错点：
//   1. 字符串下标从 0 开始，next 的边界是 next[0] = 0。
//   2. 失配时 j = next[j-1]（回退到上一个位置的前缀长度），不是 next[j]。
//   3. 每次完整匹配后 j 也要回退，否则会漏掉重叠的出现位置。
//   4. s.size() 是 size_t（无符号），减法前先转成 int。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/13_string_kmp.cpp -o /tmp/demo13
//   /tmp/demo13
// ============================================================

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ------------------------------------------------------------
// 1. 构建 next 数组
//
// 做法：
//   用两个指针：i 从 1 开始扫描模式串，j 表示"当前匹配到的前缀长度"。
//   - 如果 pattern[i] == pattern[j]，前缀长度 +1，next[i] = j；
//   - 如果不等且 j > 0，就让 j 退回到 next[j-1] 继续比较；
//   - 如果 j 已经是 0，next[i] = 0。
//
// 时间复杂度 O(m)，空间复杂度 O(m)。
// ------------------------------------------------------------
vector<int> buildNext(const string& pattern) {
    int m = (int)pattern.size();
    vector<int> next(m, 0);
    int prefixLength = 0;  // 也就是上面的 j

    for (int i = 1; i < m; ++i) {
        while (prefixLength > 0 && pattern[i] != pattern[prefixLength]) {
            prefixLength = next[prefixLength - 1];  // 回退
        }
        if (pattern[i] == pattern[prefixLength]) {
            ++prefixLength;
        }
        next[i] = prefixLength;
    }
    return next;
}

// ------------------------------------------------------------
// 2. KMP 匹配：返回 pattern 在 text 中出现的所有起始下标
//
// 思路：
//   i 扫描主串（永不回退），j 是模式串已匹配的长度。
//   失配时 j = next[j-1]；匹配满 m 个字符就记录答案，
//   然后 j 退回 next[j-1] 继续找下一个（这样能找出重叠的匹配）。
//
// 时间复杂度 O(n + m)，空间复杂度 O(m)。
// ------------------------------------------------------------
vector<int> kmpSearch(const string& text, const string& pattern) {
    vector<int> positions;
    if (pattern.empty()) return positions;

    vector<int> next = buildNext(pattern);
    int matched = 0;  // 模式串已经匹配了多长

    for (int i = 0; i < (int)text.size(); ++i) {
        while (matched > 0 && text[i] != pattern[matched]) {
            matched = next[matched - 1];  // 失配，回退
        }
        if (text[i] == pattern[matched]) {
            ++matched;
        }
        if (matched == (int)pattern.size()) {
            positions.push_back(i - matched + 1);  // 匹配的起始下标
            matched = next[matched - 1];            // 继续找下一个
        }
    }
    return positions;
}

// ------------------------------------------------------------
// 3. 暴力匹配（用来对照，理解 KMP 优化了什么）
//
// 时间复杂度 O(n * m)，空间复杂度 O(1)。
// 数据量小时和 KMP 结果必须完全一致。
// ------------------------------------------------------------
vector<int> bruteForceSearch(const string& text, const string& pattern) {
    vector<int> positions;
    if (pattern.empty()) return positions;
    int n = (int)text.size();
    int m = (int)pattern.size();

    for (int start = 0; start + m <= n; ++start) {
        int k = 0;
        while (k < m && text[start + k] == pattern[k]) ++k;
        if (k == m) positions.push_back(start);
    }
    return positions;
}

// ------------------------------------------------------------
// 4. 最长公共前缀（LeetCode 14）
//
// 思路：
//   以第一个字符串为基准，逐列比较所有字符串的第 i 个字符。
//   一旦发现某个字符串长度不够或字符不同，就返回当前已确认的前缀。
//
// 时间复杂度 O(所有字符串总长度)，空间复杂度 O(1)。
// ------------------------------------------------------------
string longestCommonPrefix(const vector<string>& words) {
    if (words.empty()) return "";

    for (int i = 0; i < (int)words[0].size(); ++i) {
        char expected = words[0][i];
        for (int k = 1; k < (int)words.size(); ++k) {
            if (i >= (int)words[k].size() || words[k][i] != expected) {
                return words[0].substr(0, i);
            }
        }
    }
    return words[0];
}

// ------------------------------------------------------------
// 5. 字符串相加（LeetCode 415）—— 模拟竖式加法
//
// 思路：
//   从两个字符串的末尾开始逐位相加，carry 记录进位。
//   用 += 往结果里追加字符，最后整体反转。
//
// 时间复杂度 O(max(n, m))，空间复杂度 O(max(n, m))。
// 易错点：不能直接 stoll，字符串可能有几十位，会溢出。
// ------------------------------------------------------------
string addStrings(const string& first, const string& second) {
    int i = (int)first.size() - 1;
    int j = (int)second.size() - 1;
    int carry = 0;
    string result;

    while (i >= 0 || j >= 0 || carry > 0) {
        int digitA = (i >= 0) ? first[i] - '0' : 0;
        int digitB = (j >= 0) ? second[j] - '0' : 0;
        int sum = digitA + digitB + carry;

        result.push_back(char('0' + sum % 10));
        carry = sum / 10;
        --i;
        --j;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------------
// 6. 验证回文串（LeetCode 125）
//
// 思路：
//   左右指针向中间走，跳过非字母数字字符（用 <cctype> 的函数），
//   统一转成小写后比较。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 易错点：isdigit / tolower 的参数要转成 unsigned char，
//   直接传负 char 是未定义行为。
// ------------------------------------------------------------

bool isPalindromeText(const string& text) {
    int left = 0;
    int right = (int)text.size() - 1;

    while (left < right) {
        while (left < right &&
               !std::isalnum((unsigned char)text[left])) {
            ++left;
        }
        while (left < right &&
               !std::isalnum((unsigned char)text[right])) {
            --right;
        }
        char leftChar = (char)std::tolower((unsigned char)text[left]);
        char rightChar = (char)std::tolower((unsigned char)text[right]);
        if (leftChar != rightChar) return false;
        ++left;
        --right;
    }
    return true;
}

int main() {
    // 示例 1：构建 next 数组
    string pattern = "ababaca";
    vector<int> next = buildNext(pattern);
    cout << "模式串 \"" << pattern << "\" 的 next 数组：";
    for (int value : next) cout << value << " ";
    cout << endl;
    assert((next == vector<int>{0, 0, 1, 2, 3, 0, 1}));

    // 示例 2：KMP 搜索
    string text = "ababababca";
    vector<int> positions = kmpSearch(text, "abab");
    cout << "在 \"" << text << "\" 中找 \"abab\" -> 起始下标：";
    for (int position : positions) cout << position << " ";
    cout << endl;
    assert((positions == vector<int>{0, 2, 4}));  // 允许重叠

    // 示例 3：和暴力匹配对拍
    vector<string> testTexts = {"aaaaa", "abcabcabc", "ababab", "xyz", ""};
    vector<string> testPatterns = {"aa", "abc", "ab", "x", "a"};
    for (const string& oneText : testTexts) {
        for (const string& onePattern : testPatterns) {
            vector<int> kmpResult = kmpSearch(oneText, onePattern);
            vector<int> bruteResult = bruteForceSearch(oneText, onePattern);
            assert(kmpResult == bruteResult);
        }
    }
    cout << "KMP 与暴力匹配在 " << testTexts.size() * testPatterns.size()
         << " 组用例上结果一致" << endl;

    // 示例 4：最长公共前缀
    vector<string> words = {"flower", "flow", "flight"};
    cout << "最长公共前缀：[\"flower\", \"flow\", \"flight\"] -> \""
         << longestCommonPrefix(words) << "\"" << endl;
    assert(longestCommonPrefix(words) == "fl");
    assert(longestCommonPrefix({"dog", "racecar", "car"}) == "");

    // 示例 5：字符串相加
    cout << "字符串相加：\"456\" + \"77\" = " << addStrings("456", "77") << endl;
    assert(addStrings("456", "77") == "533");
    assert(addStrings("0", "0") == "0");

    // 示例 6：验证回文串
    cout << "回文串判断：\"A man, a plan, a canal: Panama\" -> "
         << (isPalindromeText("A man, a plan, a canal: Panama") ? "是" : "不是")
         << "，\"race a car\" -> "
         << (isPalindromeText("race a car") ? "是" : "不是") << endl;
    assert(isPalindromeText("A man, a plan, a canal: Panama") == true);
    assert(isPalindromeText("race a car") == false);

    cout << "13 字符串与 KMP：全部示例通过" << endl;
    return 0;
}
