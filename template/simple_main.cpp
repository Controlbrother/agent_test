// ============================================================
// 可选：机试用的单文件模板（简单语法版）
//
// 说明：
//   模板/main.cpp 是"竞赛风格"的版本（bits/stdc++.h、宏、快读快写），
//   只有机试明确卡输入输出速度时才需要。本文件是它的
//   **简单语法 + 中文注释**版本，用来对照理解原模板在做什么，
//   也适合直接拿去当机试的起手文件。
//
//   和原模板的区别：
//     - 用显式 #include 代替 bits/stdc++.h；
//     - 用 cin / cout 代替手写缓冲区（数据量到 1e5 级别完全够用）；
//     - 去掉全部 #define 宏，改成普通函数；
//     - 不写 dbg / CHECK 宏，调试直接 printf / cout。
//
// 用法：把 solve() 里的内容换成题目要求的逻辑即可。
//
// 编译：
//   g++ -std=c++17 -Wall -Wextra -O2 template/simple_main.cpp -o /tmp/simple
//   /tmp/simple < template/sample_in.txt
// ============================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// 数据类型说明：
//   int 大约到 2e9，long long 大约到 9e18。
//   看到 n 个 1e9 相加、或者两个 1e9 相乘，就直接用 long long。
using int64 = long long;

// 常用常量：不要用魔法数字
const int64 INF = 1000000000000000000LL;  // 10^18，当作"无穷大"
const int64 MOD = 1000000007LL;           // 1e9 + 7

// ------------------------------------------------------------
// 快速幂：计算 base^exponent % mod
// 时间复杂度 O(log exponent)
// ------------------------------------------------------------
int64 fastPower(int64 base, int64 exponent, int64 mod) {
    int64 result = 1 % mod;
    base %= mod;
    while (exponent > 0) {
        if (exponent % 2 == 1) result = result * base % mod;
        base = base * base % mod;
        exponent /= 2;
    }
    return result;
}

// ------------------------------------------------------------
// 最大公约数（辗转相除），顺便提供最小公倍数
// ------------------------------------------------------------
int64 greatestCommonDivisor(int64 a, int64 b) {
    while (b != 0) {
        int64 remainder = a % b;
        a = b;
        b = remainder;
    }
    return a < 0 ? -a : a;
}

int64 leastCommonMultiple(int64 a, int64 b) {
    if (a == 0 || b == 0) return 0;
    return a / greatestCommonDivisor(a, b) * b;
}

// ------------------------------------------------------------
// 整数二分：在 [low, high) 里找第一个满足条件的下标
//
// 条件必须单调（前面全不满足、后面全满足），否则结果没有意义。
// 找不到时返回 high。
// ------------------------------------------------------------
template <class Predicate>
int64 firstTrue(int64 low, int64 high, Predicate predicate) {
    while (low < high) {
        int64 middle = low + (high - low) / 2;  // 防溢出，别写 (low + high) / 2
        if (predicate(middle)) {
            high = middle;   // 满足，答案可能在左半边
        } else {
            low = middle + 1;  // 不满足，答案一定在右半边
        }
    }
    return low;
}

// ------------------------------------------------------------
// 解题区：只改这里的逻辑
//
// 默认示例：读入 n 和 n 个整数，输出"总和"和"最大值"。
// ------------------------------------------------------------
void solve() {
    int n = 0;
    if (!(cin >> n) || n <= 0) return;  // 空输入直接返回，不输出脏数据

    vector<int64> numbers(n);
    for (int i = 0; i < n; ++i) cin >> numbers[i];

    int64 sum = 0;
    int64 maximum = numbers[0];  // 初值取第一个元素，数组全是负数也不会错
    for (int64 value : numbers) {
        sum += value;
        maximum = std::max(maximum, value);
    }

    cout << sum << " " << maximum << endl;
}

int main() {
    // 多组输入：把 1 改成读入的测试组数 T 即可
    int testCases = 1;
    // cin >> testCases;
    while (testCases--) {
        solve();
    }
    return 0;
}
