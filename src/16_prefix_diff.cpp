// ============================================================
// 专题 16：前缀和 / 差分
//
// 适用题型：
//   前缀和：区间求和（LeetCode 303）、二维区域和（304）、
//           和为 K 的子数组（560）、除自身以外数组的乘积（238）；
//   差分：  区间批量加（LeetCode 1109 航班预订统计）、二维差分。
//
// 核心思想：
//   - 前缀和：把"区间求和"从 O(n) 降到 O(1)，
//     代价是预处理 O(n)。pre[i] = 前 i 个元素的和。
//   - 差分：把"区间加"从 O(n) 降到 O(1)，
//     最后再做一遍前缀和还原成原数组。
//     对 [l, r] 加 v 就是 diff[l] += v、diff[r+1] -= v。
//
// 时间复杂度：预处理 O(n)，每次查询 / 修改 O(1)
// 空间复杂度：O(n)（二维是 O(m*n)）
//
// 易错点：
//   1. 前缀和数组开 n + 1 位，pre[0] = 0，
//      这样 [l, r] 的公式 pre[r+1] - pre[l] 不用特判 l = 0。
//   2. 二维前缀和的容斥符号是 + - - +，写反答案就错。
//   3. 差分的 r + 1 可能越界，数组要多开一位。
//   4. 数据大时用 long long：n 个 1e9 相加会超 int。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/16_prefix_diff.cpp -o /tmp/demo16
//   /tmp/demo16
// ============================================================

#include <cassert>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// ------------------------------------------------------------
// 1. 一维前缀和
//
// pre[i] 表示 a[0] + a[1] + ... + a[i-1]（前 i 个元素的和）。
// 那么区间 [left, right] 的和 = pre[right + 1] - pre[left]。
//
// 时间复杂度 预处理 O(n)、查询 O(1)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<long long> buildPrefixSum(const vector<int>& numbers) {
    int n = (int)numbers.size();
    vector<long long> prefix(n + 1, 0);  // 多开一位，pre[0] = 0

    for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + numbers[i];
    }
    return prefix;
}

long long rangeSum(const vector<long long>& prefix, int left, int right) {
    return prefix[right + 1] - prefix[left];
}

// ------------------------------------------------------------
// 2. 二维前缀和
//
// pre[i][j] 表示左上角 (0, 0) 到 (i-1, j-1) 这个矩形的和。
// 递推：pre[i][j] = grid[i-1][j-1] + pre[i-1][j] + pre[i][j-1] - pre[i-1][j-1]
// 查询 (r1, c1) 到 (r2, c2)：
//   pre[r2+1][c2+1] - pre[r1][c2+1] - pre[r2+1][c1] + pre[r1][c1]
// 记忆方法：大矩形 减 上面一条 减 左边一条 加 多减的左上角。
//
// 时间复杂度 预处理 O(m*n)、查询 O(1)，空间复杂度 O(m*n)。
// ------------------------------------------------------------
vector<vector<long long>> buildPrefixSum2D(const vector<vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) return {};

    int rows = (int)grid.size();
    int cols = (int)grid[0].size();
    vector<vector<long long>> prefix(rows + 1, vector<long long>(cols + 1, 0));

    for (int i = 1; i <= rows; ++i) {
        for (int j = 1; j <= cols; ++j) {
            prefix[i][j] = grid[i - 1][j - 1] + prefix[i - 1][j] +
                           prefix[i][j - 1] - prefix[i - 1][j - 1];
        }
    }
    return prefix;
}

long long rangeSum2D(const vector<vector<long long>>& prefix,
                     int row1, int col1, int row2, int col2) {
    return prefix[row2 + 1][col2 + 1] - prefix[row1][col2 + 1] -
           prefix[row2 + 1][col1] + prefix[row1][col1];
}

// ------------------------------------------------------------
// 3. 差分数组：区间加
//
// 对 [left, right] 全部加 value，只需要 diff[left] += value,
// diff[right + 1] -= value。最后做一次前缀和就还原出每个位置的总增量。
//
// 时间复杂度 每次修改 O(1)、还原 O(n)，空间复杂度 O(n)。
//
// 易错点：diff 数组要开 n + 1 位，right + 1 == n 时也不越界。
// ------------------------------------------------------------
vector<long long> buildDifferenceArray(const vector<int>& numbers) {
    int n = (int)numbers.size();
    vector<long long> diff(n + 1, 0);  // 多开一位给 right + 1

    diff[0] = numbers[0];
    for (int i = 1; i < n; ++i) {
        diff[i] = numbers[i] - numbers[i - 1];
    }
    return diff;
}

void rangeAdd(vector<long long>& diff, int left, int right, long long value) {
    diff[left] += value;
    diff[right + 1] -= value;  // 从 right + 1 开始"抵消"
}

vector<long long> restoreFromDifference(const vector<long long>& diff, int n) {
    vector<long long> numbers(n, 0);
    long long running = 0;
    for (int i = 0; i < n; ++i) {
        running += diff[i];
        numbers[i] = running;
    }
    return numbers;
}

// ------------------------------------------------------------
// 4. 应用：航班预订统计（LeetCode 1109）
//
// 题目：bookings[i] = [first, last, seats] 表示从 first 到 last
//       （航班编号 1 到 n，闭区间）每个航班增加 seats 个座位。
//       求每个航班最终的座位数。
//
// 思路：标准的差分模板题。航班编号从 1 开始，
//   所以差分数组用 1..n+1 的偏移写法最直观。
//
// 时间复杂度 O(n + m)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<long long> corpFlightBookings(const vector<vector<int>>& bookings,
                                     int n) {
    vector<long long> diff(n + 2, 0);  // 下标 1..n，多开一位给 r+1

    for (const vector<int>& booking : bookings) {
        int first = booking[0];
        int last = booking[1];
        int seats = booking[2];
        diff[first] += seats;
        diff[last + 1] -= seats;
    }

    vector<long long> answer(n, 0);
    long long running = 0;
    for (int flight = 1; flight <= n; ++flight) {
        running += diff[flight];
        answer[flight - 1] = running;
    }
    return answer;
}

int main() {
    // 示例 1：一维前缀和
    vector<int> numbers = {1, 2, 3, 4, 5};
    vector<long long> prefix = buildPrefixSum(numbers);
    cout << "一维前缀和：数组 [1, 2, 3, 4, 5]，区间 [1, 3] 的和 = "
         << rangeSum(prefix, 1, 3) << endl;
    assert(rangeSum(prefix, 1, 3) == 9);    // 2 + 3 + 4
    assert(rangeSum(prefix, 0, 4) == 15);   // 整个数组
    assert(rangeSum(prefix, 2, 2) == 3);    // 单个元素

    // 示例 2：二维前缀和
    vector<vector<int>> grid = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
    };
    vector<vector<long long>> prefix2D = buildPrefixSum2D(grid);
    long long answer2D = rangeSum2D(prefix2D, 1, 1, 2, 2);
    cout << "二维前缀和：3x5 网格中 (1,1) 到 (2,2) 的矩形和 = "
         << answer2D << endl;
    assert(answer2D == 11);  // 6 + 3 + 2 + 0
    assert(rangeSum2D(prefix2D, 0, 0, 0, 0) == 3);

    // 示例 3：差分数组
    vector<int> base = {1, 2, 3, 4, 5};
    vector<long long> diff = buildDifferenceArray(base);
    // 对 [1, 3] 加 10，对 [0, 1] 加 5
    rangeAdd(diff, 1, 3, 10);
    rangeAdd(diff, 0, 1, 5);
    vector<long long> restored = restoreFromDifference(diff, (int)base.size());
    cout << "差分：原数组 [1, 2, 3, 4, 5]，[1,3] 加 10、[0,1] 加 5 后 -> ";
    for (int i = 0; i < (int)restored.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << restored[i];
    }
    cout << endl;
    assert((restored == vector<long long>{6, 17, 13, 14, 5}));  // 手工核对

    // 示例 4：航班预订统计
    vector<vector<int>> bookings = {{1, 2, 10}, {2, 3, 20}, {2, 5, 25}};
    vector<long long> seats = corpFlightBookings(bookings, 5);
    cout << "航班预订：n = 5，bookings = [[1,2,10], [2,3,20], [2,5,25]] -> ";
    for (int i = 0; i < (int)seats.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << seats[i];
    }
    cout << endl;
    assert((seats == vector<long long>{10, 55, 45, 25, 25}));

    // 边界：空数组、单元素
    assert(buildPrefixSum({}).size() == 1);          // 只有 pre[0] = 0
    assert(rangeSum(buildPrefixSum({}), 0, -1) == 0);  // 空区间
    assert(buildPrefixSum2D({}).empty());

    cout << "16 前缀和与差分：全部示例通过" << endl;
    return 0;
}
