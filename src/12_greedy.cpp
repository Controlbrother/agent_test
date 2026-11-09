// ============================================================
// 专题 12：贪心
//
// 适用题型：区间调度、合并区间、无重叠区间、跳跃游戏、
//           买卖股票的最佳时机、加油站、划分字母区间。
//
// 核心思想：
//   每一步都选"当前看起来最优"的，并且能证明局部最优能推出全局最优。
//   面试里判断能不能用贪心的关键：能不能举出反例。
//   举不出反例 + 能说清"为什么这样选不会更差"，就可以用。
//
// 时间复杂度：通常是排序 O(n log n) + 一次遍历 O(n)
// 空间复杂度：O(1) 或 O(n)
//
// 易错点：
//   1. 区间问题排序依据不同：
//        - 求"最多不重叠" -> 按结束时间升序；
//        - 求"合并区间"   -> 按开始时间升序。
//      排错了答案就错。
//   2. 贪心只给结论不给证明，面试时要能口头解释为什么。
//   3. 边界：区间首尾相接算不算重叠、空数组要单独返回。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/12_greedy.cpp -o /tmp/demo12
//   /tmp/demo12
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
// 1. 区间调度：最多能参加多少个不重叠的会议
//
// 思路：
//   按"结束时间"升序排序，能参加就参加（开始时间 >= 上一个结束时间）。
//   为什么按结束时间？结束得越早，后面能安排的空间越大。
//   按开始时间排则是错的，反例：[[0,10], [1,2], [2,3]]。
//
// 时间复杂度 O(n log n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int maxMeetings(vector<vector<int>> meetings) {
    std::sort(meetings.begin(), meetings.end(),
              [](const vector<int>& a, const vector<int>& b) {
                  return a[1] < b[1];  // 按结束时间升序
              });

    int count = 0;
    int lastEnd = INT_MIN;
    for (const vector<int>& meeting : meetings) {
        if (meeting[0] >= lastEnd) {  // 与上一个不重叠
            ++count;
            lastEnd = meeting[1];
        }
    }
    return count;
}

// ------------------------------------------------------------
// 2. 合并区间（LeetCode 56）
//
// 思路：
//   按"开始时间"升序排序，然后依次处理：
//   - 如果当前区间和结果里最后一个区间重叠，就合并（更新右端点）；
//   - 否则把当前区间加进结果。
//
// 时间复杂度 O(n log n)，空间复杂度 O(n)（存结果）。
// ------------------------------------------------------------
vector<vector<int>> mergeIntervals(vector<vector<int>> intervals) {
    if (intervals.empty()) return {};

    std::sort(intervals.begin(), intervals.end(),
              [](const vector<int>& a, const vector<int>& b) {
                  return a[0] < b[0];  // 按开始时间升序
              });

    vector<vector<int>> merged;
    for (const vector<int>& interval : intervals) {
        if (merged.empty() || interval[0] > merged.back()[1]) {
            merged.push_back(interval);  // 不重叠，直接加入
        } else {
            // 重叠，右端点取较大值
            merged.back()[1] = std::max(merged.back()[1], interval[1]);
        }
    }
    return merged;
}

// ------------------------------------------------------------
// 3. 跳跃游戏（LeetCode 55）
//
// 思路：
//   维护"当前能到达的最远位置" farthest。
//   如果遍历到的下标已经超过 farthest，说明这个位置根本走不到，返回 false。
//   否则用 i + nums[i] 更新 farthest。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
bool canJump(const vector<int>& jumps) {
    int farthest = 0;
    for (int i = 0; i < (int)jumps.size(); ++i) {
        if (i > farthest) return false;  // 这一步根本走不到
        farthest = std::max(farthest, i + jumps[i]);
    }
    return true;
}

// ------------------------------------------------------------
// 4. 跳跃游戏 II：最少跳几次（LeetCode 45）
//
// 思路：
//   把"能跳到的范围"当成一层一层的区间：
//   currentEnd 是当前这一跳能到的最远位置，farthest 是下一跳能到的最远位置。
//   走到 currentEnd 就必须多跳一次，然后 currentEnd 更新为 farthest。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
//
// 前提：题目（LeetCode 45）保证一定可以到达终点。
//   如果输入本身不可达，这个函数不会报错，但返回值没有意义
//   —— 需要判断可达性时请先用 canJump() 检查。
// ------------------------------------------------------------
int jumpTimes(const vector<int>& jumps) {
    int jumpsUsed = 0;
    int currentEnd = 0;   // 当前这一跳能到的最远处
    int farthest = 0;     // 下一跳能到的最远处

    for (int i = 0; i + 1 < (int)jumps.size(); ++i) {
        farthest = std::max(farthest, i + jumps[i]);
        if (i == currentEnd) {   // 必须再跳一次
            ++jumpsUsed;
            currentEnd = farthest;
        }
    }
    return jumpsUsed;
}

// ------------------------------------------------------------
// 5. 买卖股票的最佳时机（LeetCode 121）—— 只能买卖一次
//
// 思路：
//   遍历价格，维护"到目前为止的最低价格"，
//   每天用"今天的价格 - 最低价格"更新最大利润。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int maxProfit(const vector<int>& prices) {
    int lowest = INT_MAX;
    int bestProfit = 0;
    for (int price : prices) {
        lowest = std::min(lowest, price);
        bestProfit = std::max(bestProfit, price - lowest);
    }
    return bestProfit;
}

// ------------------------------------------------------------
// 6. 加油站（LeetCode 134）
//
// 思路：
//   如果总油量 < 总消耗，一定无解。
//   否则从某个起点出发：一旦"当前剩余油量"变成负数，
//   说明从起点到当前点之间的任何点出发都不行，起点直接跳到下一个点。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int canCompleteCircuit(const vector<int>& gas, const vector<int>& cost) {
    long long totalGas = 0;
    long long totalCost = 0;
    for (int value : gas) totalGas += value;
    for (int value : cost) totalCost += value;
    if (totalGas < totalCost) return -1;

    int start = 0;
    long long tank = 0;
    for (int i = 0; i < (int)gas.size(); ++i) {
        tank += gas[i] - cost[i];
        if (tank < 0) {   // 从 start 出发到不了 i + 1
            start = i + 1;
            tank = 0;
        }
    }
    return start;
}

// ------------------------------------------------------------
// 7. 划分字母区间（LeetCode 763）
//
// 思路：
//   先记录每个字母最后出现的位置。
//   再从头遍历，用 end 记录"当前片段必须延伸到的最远处"。
//   当遍历到 end 时，说明这个片段可以切了。
//
// 时间复杂度 O(n)，空间复杂度 O(1)（26 个字母）。
// ------------------------------------------------------------
vector<int> partitionLabels(const string& text) {
    int lastPosition[26];
    for (int i = 0; i < 26; ++i) lastPosition[i] = -1;
    for (int i = 0; i < (int)text.size(); ++i) {
        lastPosition[text[i] - 'a'] = i;
    }

    vector<int> result;
    int start = 0;
    int end = 0;
    for (int i = 0; i < (int)text.size(); ++i) {
        end = std::max(end, lastPosition[text[i] - 'a']);
        if (i == end) {              // 这个片段里的字母都收齐了
            result.push_back(end - start + 1);
            start = end + 1;
        }
    }
    return result;
}

// ------------------------------------------------------------
// 8. 最长递增子序列 O(n log n) 版本（贪心 + 二分）
//
// 思路：
//   tails[k] 表示"长度 k+1 的递增子序列"的末尾元素最小值。
//   遍历每个数字 x，用二分找到 tails 中第一个 >= x 的位置并替换它。
//   tails 的长度就是 LIS 长度。
//
// 注意：tails 不是真正的 LIS，只是长度相同；要还原序列需要额外记录。
//
// 时间复杂度 O(n log n)，空间复杂度 O(n)。
// ------------------------------------------------------------
int lengthOfLISFast(const vector<int>& numbers) {
    vector<int> tails;  // tails 始终严格递增
    for (int value : numbers) {
        auto position = std::lower_bound(tails.begin(), tails.end(), value);
        if (position == tails.end()) {
            tails.push_back(value);   // 比所有末尾都大，序列变长
        } else {
            *position = value;        // 替换成更小的末尾，后面更容易接
        }
    }
    return (int)tails.size();
}

// 打印二维数组（合并区间的结果）
void printIntervals(const vector<vector<int>>& intervals) {
    for (const vector<int>& one : intervals) {
        cout << "[" << one[0] << ", " << one[1] << "] ";
    }
    cout << endl;
}

int main() {
    // 示例 1：区间调度
    vector<vector<int>> meetings = {{1, 4}, {3, 5}, {0, 6}, {5, 7}, {3, 9},
                                    {5, 9}, {6, 10}, {8, 11}, {8, 12}, {2, 14},
                                    {12, 16}};
    int meetingCount = maxMeetings(meetings);
    cout << "区间调度：最多参加 " << meetingCount << " 个会议" << endl;
    assert(meetingCount == 4);

    // 示例 2：合并区间
    vector<vector<int>> intervals = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    vector<vector<int>> merged = mergeIntervals(intervals);
    cout << "合并区间：";
    printIntervals(merged);
    assert((merged == vector<vector<int>>{{1, 6}, {8, 10}, {15, 18}}));

    // 示例 3：跳跃游戏
    cout << "跳跃游戏：[2, 3, 1, 1, 4] 能到达终点 = "
         << (canJump({2, 3, 1, 1, 4}) ? "是" : "否")
         << "，[3, 2, 1, 0, 4] 能到达终点 = "
         << (canJump({3, 2, 1, 0, 4}) ? "是" : "否") << endl;
    assert(canJump({2, 3, 1, 1, 4}) == true);
    assert(canJump({3, 2, 1, 0, 4}) == false);

    // 示例 4：最少跳跃次数
    cout << "最少跳跃次数：[2, 3, 1, 1, 4] -> " << jumpTimes({2, 3, 1, 1, 4})
         << " 次" << endl;
    assert(jumpTimes({2, 3, 1, 1, 4}) == 2);
    // 不可达的输入（如 [0, 1]）返回值无意义，用 canJump 先判断
    assert(canJump({0, 1}) == false);

    // 示例 5：买卖股票
    cout << "买卖股票：[7, 1, 5, 3, 6, 4] -> 最大利润 "
         << maxProfit({7, 1, 5, 3, 6, 4}) << endl;
    assert(maxProfit({7, 1, 5, 3, 6, 4}) == 5);  // 1 买入 6 卖出
    assert(maxProfit({7, 6, 4, 3, 1}) == 0);      // 一直跌，不交易

    // 示例 6：加油站
    cout << "加油站：gas = [1, 2, 3, 4, 5]，cost = [3, 4, 5, 1, 2] -> 起点 "
         << canCompleteCircuit({1, 2, 3, 4, 5}, {3, 4, 5, 1, 2}) << endl;
    assert(canCompleteCircuit({1, 2, 3, 4, 5}, {3, 4, 5, 1, 2}) == 3);
    assert(canCompleteCircuit({2, 3, 4}, {3, 4, 3}) == -1);

    // 示例 7：划分字母区间
    vector<int> parts = partitionLabels("ababcbacadefegdehijhklij");
    cout << "划分字母区间：\"ababcbacadefegdehijhklij\" -> ";
    for (int length : parts) cout << length << " ";
    cout << endl;
    assert((parts == vector<int>{9, 7, 8}));

    // 示例 8：LIS 贪心 + 二分
    vector<int> numbers = {10, 9, 2, 5, 3, 7, 101, 18};
    cout << "LIS（贪心 + 二分）：长度 = " << lengthOfLISFast(numbers) << endl;
    assert(lengthOfLISFast(numbers) == 4);
    assert(lengthOfLISFast({5, 4, 3, 2, 1}) == 1);

    cout << "12 贪心：全部示例通过" << endl;
    return 0;
}
