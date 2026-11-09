// ============================================================
// 专题 04：哈希表
//
// 适用题型：两数之和、字母异位词分组、最长连续序列、
//           和为 K 的子数组（前缀和 + 哈希）等。
//
// 核心思想：
//   用空间换时间。把"查找某个值是否出现过"从 O(n) 降到 O(1)，
//   所以大量"两数/三数/配对"题目写成"一边遍历，一边把见过的
//   东西存进哈希表"就能从 O(n^2) 降到 O(n)。
//
// 常用容器：
//   std::unordered_map<Key, Value>：键值对，查找 O(1)（平均）
//   std::unordered_set<Key>       ：只关心"出现过没有"
//
// 时间复杂度：平均 O(n)，最坏 O(n^2)（哈希冲突极端时）
// 空间复杂度：O(n)
//
// 易错点：
//   1. 两数之和必须"先查后存"，否则 target = 2 * x 时会用到自己。
//   2. 用 mp[key] 会自动插入默认值，只想查询时用 mp.find(key)。
//   3. 边遍历边插入可能导致迭代器失效，需要时先收集再插入。
//   4. unordered_map 的 key 必须可哈希，pair 不行，要自己转换。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/04_hash_table.cpp -o /tmp/demo04
//   /tmp/demo04
// ============================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// ------------------------------------------------------------
// 题目 1：两数之和（LeetCode 1）
//
// 思路：
//   遍历数组，对每个 numbers[i]，先看 target - numbers[i] 是否出现过；
//   出现过就返回两个下标。然后把 numbers[i] 存进哈希表。
//   注意顺序：先查后存，避免把当前元素自己当成答案。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
// 没找到时返回空数组。
// ------------------------------------------------------------
vector<int> twoSum(const vector<int>& numbers, int target) {
    unordered_map<int, int> valueToIndex;  // 值 -> 下标

    for (int i = 0; i < (int)numbers.size(); ++i) {
        int needed = target - numbers[i];
        auto found = valueToIndex.find(needed);  // 先查
        if (found != valueToIndex.end()) {
            return {found->second, i};
        }
        valueToIndex[numbers[i]] = i;            // 后存
    }
    return {};
}

// ------------------------------------------------------------
// 题目 2：字母异位词分组（LeetCode 49）
//
// 思路：
//   两个词是字母异位词 <=> 它们排序后的字符串相同。
//   把"排序后的字符串"当 key，原字符串放进对应的组里。
//
// 时间复杂度 O(n * m log m)，m 是最长单词长度（排序开销）
// 空间复杂度 O(n * m)
// ------------------------------------------------------------
vector<vector<string>> groupAnagrams(const vector<string>& words) {
    unordered_map<string, vector<string>> groups;

    for (const string& word : words) {
        string key = word;
        std::sort(key.begin(), key.end());  // 排序后的串当 key
        groups[key].push_back(word);
    }

    vector<vector<string>> result;
    for (auto& entry : groups) {
        result.push_back(entry.second);
    }
    return result;
}

// ------------------------------------------------------------
// 题目 3：最长连续序列（LeetCode 128）
//
// 思路：
//   把全部数字放进 unordered_set，然后只从"序列的起点"开始数：
//   如果 x - 1 不在集合里，说明 x 是一段连续序列的起点，
//   从 x 开始一直 +1 看能延伸多长。
//   每个数字最多被访问两次，所以是 O(n)。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
// ------------------------------------------------------------
int longestConsecutive(const vector<int>& numbers) {
    unordered_set<int> values(numbers.begin(), numbers.end());
    int best = 0;

    for (int value : values) {
        if (values.count(value - 1) > 0) {
            continue;  // 不是起点，跳过
        }
        int current = value;
        int length = 1;
        while (values.count(current + 1) > 0) {
            ++current;
            ++length;
        }
        best = std::max(best, length);
    }
    return best;
}

// ------------------------------------------------------------
// 题目 4：和为 K 的子数组（LeetCode 560）—— 前缀和 + 哈希
//
// 思路：
//   区间 [i+1, j] 的和 = prefix[j] - prefix[i]。
//   要让它等于 k，等价于 prefix[i] == prefix[j] - k。
//   所以遍历 j 时，用哈希表记录之前每个前缀和出现的次数，
//   答案累加 count[prefix[j] - k]。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
//
// 易错点：
//   哈希表要预先放入 count[0] = 1，表示"空前缀"，
//   否则从下标 0 开始的子数组会被漏掉。
// ------------------------------------------------------------
int subarraySum(const vector<int>& numbers, int k) {
    unordered_map<long long, int> prefixCount;  // 前缀和 -> 出现次数
    prefixCount[0] = 1;

    long long prefix = 0;
    int answer = 0;
    for (int value : numbers) {
        prefix += value;
        long long needed = prefix - k;
        auto found = prefixCount.find(needed);
        if (found != prefixCount.end()) {
            answer += found->second;
        }
        ++prefixCount[prefix];
    }
    return answer;
}

int main() {
    // 示例 1：两数之和
    vector<int> numbers1 = {2, 7, 11, 15};
    vector<int> answer1 = twoSum(numbers1, 9);
    cout << "两数之和：nums = [2, 7, 11, 15]，target = 9 -> 下标 ["
         << answer1[0] << ", " << answer1[1] << "]" << endl;
    assert((answer1 == vector<int>{0, 1}));

    // 示例 2：字母异位词分组
    vector<string> words = {"eat", "tea", "tan", "ate", "nat", "bat"};
    vector<vector<string>> groups = groupAnagrams(words);
    cout << "字母异位词分组：共 " << groups.size() << " 组" << endl;
    assert(groups.size() == 3);

    // 示例 3：最长连续序列
    vector<int> numbers2 = {100, 4, 200, 1, 3, 2};
    int answer3 = longestConsecutive(numbers2);
    cout << "最长连续序列：nums = [100, 4, 200, 1, 3, 2] -> 长度 = "
         << answer3 << endl;
    assert(answer3 == 4);  // 1, 2, 3, 4

    // 示例 4：和为 K 的子数组
    vector<int> numbers3 = {1, 1, 1};
    int answer4 = subarraySum(numbers3, 2);
    cout << "和为 K 的子数组：nums = [1, 1, 1]，k = 2 -> 个数 = "
         << answer4 << endl;
    assert(answer4 == 2);

    vector<int> numbers4 = {3, 4, 7, 2, -3, 1, 4, 2};
    assert(subarraySum(numbers4, 7) == 4);  // 含负数也正确

    cout << "04 哈希表：全部示例通过" << endl;
    return 0;
}
