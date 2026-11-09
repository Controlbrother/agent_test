// ============================================================
// 专题 03：二分查找
//
// 适用题型：有序数组查找、查找左右边界、旋转数组、二分答案
//           （"最小的最大值""最大的最小值"）、实数二分、三分求极值。
//
// 核心思想：
//   每次把搜索区间砍掉一半。只要区间上的判断是"单调的"
//   （前面全是 false、后面全是 true），就可以二分。
//
// 本文件只用一种写法（左闭右开 [left, right)），避免背四种模板：
//   如果条件成立，答案在左半边 -> right = mid；
//   如果条件不成立，答案在右半边 -> left = mid + 1。
//   循环结束时 left == right == 第一个满足条件的位置。
//
// 时间复杂度：O(log n)；二分答案 O(n log 值域)
// 空间复杂度：O(1)
//
// 易错点：
//   1. mid 写成 left + (right - left) / 2，不要写 (left + right) / 2，
//      后者在 left、right 很大时会溢出。
//   2. 区间是开是闭必须全程统一，不要一会儿 [l, r] 一会儿 [l, r)。
//   3. 找不到时 left == right 是"越界位置"，调用方要判断是否 equals n。
//   4. 二分答案时 check(mid) 的方向要搞对：mid 可行就往小的方向收。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/03_binary_search.cpp -o /tmp/demo03
//   /tmp/demo03
// ============================================================

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::function;
using std::vector;

// ------------------------------------------------------------
// 1. 手写 lower_bound：在升序数组中找第一个 >= target 的下标
//
// 区间是 [left, right)，right 初值取 n（不是 n - 1），
// 这样"没找到"时返回 n，语义清晰。
//
// 时间复杂度 O(log n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int lowerBound(const vector<int>& numbers, int target) {
    int left = 0;
    int right = (int)numbers.size();  // 左闭右开

    while (left < right) {
        int mid = left + (right - left) / 2;  // 防溢出写法
        if (numbers[mid] >= target) {
            right = mid;      // mid 满足条件，答案在 [left, mid]
        } else {
            left = mid + 1;   // mid 不满足，答案在 [mid+1, right)
        }
    }
    return left;  // left == right
}

// ------------------------------------------------------------
// 2. 查找元素的第一个和最后一个位置（LeetCode 34）
//
// 思路：
//   第一个位置 = 第一个 >= target 的下标；
//   最后一个位置 = 第一个 > target 的下标 - 1。
//   两次二分，都复用 lowerBound 的思路。
//
// 时间复杂度 O(log n)，空间复杂度 O(1)。
// ------------------------------------------------------------
vector<int> searchRange(const vector<int>& numbers, int target) {
    int n = (int)numbers.size();

    // 第一个 >= target
    int left = 0;
    int right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (numbers[mid] >= target) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    int first = left;

    // 第一个 > target
    left = 0;
    right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (numbers[mid] > target) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    int last = left - 1;

    if (first > last || first == n || numbers[first] != target) {
        return {-1, -1};  // 一个都没找到
    }
    return {first, last};
}

// ------------------------------------------------------------
// 3. 二分答案：爱吃香蕉的珂珂（LeetCode 875）
//
// 题目：piles[i] 堆香蕉，每小时选一堆吃 speed 根，吃完才能换堆。
//      要求在 h 小时内吃完，求最小的 speed。
//
// 思路：speed 越大，所需时间越少，这是单调的。
//   check(speed) = 总小时数 <= h，可行就尝试更小的 speed。
//   答案范围 [1, 最大堆的香蕉数]。
//
// 时间复杂度 O(n log maxPile)，空间复杂度 O(1)。
// ------------------------------------------------------------
bool canFinish(const vector<int>& piles, int speed, int hours) {
    long long totalHours = 0;  // 用 long long，避免累加溢出
    for (int pile : piles) {
        totalHours += (pile + speed - 1) / speed;  // 向上取整
    }
    return totalHours <= hours;
}

int minEatingSpeed(const vector<int>& piles, int hours) {
    int left = 1;                                        // 最小速度 1
    int right = *std::max_element(piles.begin(), piles.end());  // 最大速度
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (canFinish(piles, mid, hours)) {
            right = mid;      // 可行，试试更小
        } else {
            left = mid + 1;   // 不可行，必须更快
        }
    }
    return left;
}

// ------------------------------------------------------------
// 4. 旋转数组中的最小值（LeetCode 153，无重复元素）
//
// 思路：
//   旋转后数组分成两段升序。比较 nums[mid] 和 nums[right]：
//   - nums[mid] > nums[right]：最小值一定在 mid 右边 -> left = mid + 1；
//   - 否则最小值在 mid 或它左边 -> right = mid。
//
// 时间复杂度 O(log n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int findMinInRotated(const vector<int>& numbers) {
    int left = 0;
    int right = (int)numbers.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (numbers[mid] > numbers[right]) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return numbers[left];
}

// ------------------------------------------------------------
// 5. 实数二分：求平方根（保留误差 1e-9）
//
// 思路：
//   在 [0, x] 上二分，mid * mid <= x 说明答案 >= mid。
//   固定循环 100 次：区间每次减半，100 次后精度足够，
//   比写 while (right - left > 1e-9) 更稳（不会因为浮点误差死循环）。
//
// 时间复杂度 O(100)，空间复杂度 O(1)。
// ------------------------------------------------------------
double squareRoot(double x) {
    if (x <= 0) return 0;
    double left = 0;
    double right = std::max(1.0, x);  // x < 1 时右边界取 1
    for (int i = 0; i < 100; ++i) {
        double mid = (left + right) / 2;
        if (mid * mid <= x) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return (left + right) / 2;
}

// ------------------------------------------------------------
// 6. 二分查找的 STL 写法（能用就直接用）
//
// 升序数组上：
//   lower_bound(begin, end, x) -> 第一个 >= x 的位置
//   upper_bound(begin, end, x) -> 第一个 > x 的位置
//   binary_search(begin, end, x) -> 是否存在
// 返回值是迭代器，减去 begin() 得到下标。
//
// 时间复杂度 O(log n)。
// ------------------------------------------------------------
void stlBinarySearchDemo() {
    vector<int> numbers = {1, 2, 2, 2, 4, 6};

    int firstNotLess = (int)(std::lower_bound(numbers.begin(), numbers.end(), 2) -
                             numbers.begin());
    int firstGreater = (int)(std::upper_bound(numbers.begin(), numbers.end(), 2) -
                             numbers.begin());
    bool exists = std::binary_search(numbers.begin(), numbers.end(), 4);

    cout << "STL：数组 [1, 2, 2, 2, 4, 6]" << endl;
    cout << "  第一个 >= 2 的下标 = " << firstNotLess
         << "，第一个 > 2 的下标 = " << firstGreater
         << "，4 是否存在 = " << (exists ? "是" : "否") << endl;

    assert(firstNotLess == 1);
    assert(firstGreater == 4);
    assert(exists);
}

// ------------------------------------------------------------
// 7. 三分查找：求单峰函数的最小值
//
// 思路：
//   函数先下降后上升（单峰）时，每次取两个三等分点 m1、m2：
//   - f(m1) < f(m2)：最小值一定在 [left, m2]；
//   - 否则最小值一定在 [m1, right]。
//   每次砍掉三分之一，收敛略慢于二分，但能处理"非单调但单峰"的函数。
//
// 时间复杂度 O(log 区间长度)，空间复杂度 O(1)。
// 这里用 f(x) = (x - 3)^2 演示，最小值在 x = 3 处。
// ------------------------------------------------------------
double parabola(double x) { return (x - 3.0) * (x - 3.0); }

double ternarySearchMinimum(double left, double right) {
    for (int i = 0; i < 200; ++i) {  // 固定迭代次数，保证一定收敛
        double m1 = left + (right - left) / 3;
        double m2 = right - (right - left) / 3;
        if (parabola(m1) < parabola(m2)) {
            right = m2;   // 最小值在 [left, m2]
        } else {
            left = m1;    // 最小值在 [m1, right]
        }
    }
    return (left + right) / 2;
}

int main() {
    vector<int> sortedNumbers = {1, 2, 2, 2, 3, 5, 8};

    // 示例 1：手写 lower_bound
    int pos = lowerBound(sortedNumbers, 3);
    cout << "手写 lower_bound：数组 [1, 2, 2, 2, 3, 5, 8]，找 3 -> 下标 "
         << pos << endl;
    assert(pos == 4);
    assert(lowerBound(sortedNumbers, 4) == 5);  // 4 不存在，返回第一个 > 4 的位置
    assert(lowerBound(sortedNumbers, 9) == 7);  // 全部小于 9，返回 n

    // 示例 2：查找第一个和最后一个位置
    vector<int> range = searchRange(sortedNumbers, 2);
    cout << "查找 2 的范围 -> [" << range[0] << ", " << range[1] << "]" << endl;
    assert((range == vector<int>{1, 3}));
    assert((searchRange(sortedNumbers, 4) == vector<int>{-1, -1}));

    // 示例 3：二分答案
    vector<int> piles = {3, 6, 7, 11};
    int speed = minEatingSpeed(piles, 8);
    cout << "珂珂吃香蕉：piles = [3, 6, 7, 11]，h = 8 -> 最小速度 = "
         << speed << endl;
    assert(speed == 4);

    // 示例 4：旋转数组求最小值
    vector<int> rotated = {4, 5, 6, 7, 0, 1, 2};
    int minimum = findMinInRotated(rotated);
    cout << "旋转数组 [4, 5, 6, 7, 0, 1, 2] 的最小值 = " << minimum << endl;
    assert(minimum == 0);
    assert(findMinInRotated(vector<int>{3, 1, 2}) == 1);  // 旋转一位
    assert(findMinInRotated(vector<int>{1, 2, 3}) == 1);  // 没旋转

    // 示例 5：实数二分
    double root = squareRoot(2.0);
    cout << "实数二分 sqrt(2) = " << root << endl;
    assert(std::fabs(root - std::sqrt(2.0)) < 1e-9);

    // 示例 6：三分查找
    double minimumPoint = ternarySearchMinimum(-10.0, 10.0);
    cout << "三分查找 (x-3)^2 的最小值点 x = " << minimumPoint << endl;
    assert(std::fabs(minimumPoint - 3.0) < 1e-6);

    // 示例 7：STL
    stlBinarySearchDemo();

    cout << "03 二分查找：全部示例通过" << endl;
    return 0;
}
