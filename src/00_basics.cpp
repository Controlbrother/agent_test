// ============================================================
// 专题 00：C++ 刷题基本功（读入 + 输出 + 防溢出）
//
// 本文件面向"算法基础弱"的同学，先解决最基础的三件事：
//   1. 怎么读入一组数、怎么输出答案；
//   2. 什么时候必须用 long long（防溢出）；
//   3. 求最大值 / 最小值时的初值怎么取。
//
// 本仓库的默认写法是"最朴素"的那种：显式 #include、std:: 前缀、
// cin / cout。只有机试明确卡输入输出速度时，才去看 template/main.cpp
// （那是可选的竞赛模板，不是日常练习代码）。
//
// 题目：读入 n 和 n 个整数，输出它们的总和与最大值。
//   输入示例（tests/samples/00_basics.in）：
//     5
//     1 2 3 4 5
//   输出示例（tests/expected/00_basics.out）：
//     15 5
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
//
// 易错点：
//   1. 求最大值的初值不能写 0：数组全是负数时答案会错，应取第一个元素。
//   2. n 个数求和、两个 1e9 相乘都可能溢出 int，要用 long long。
//   3. 读入前先判断 cin >> n 是否成功，空输入时直接返回，不输出脏数据。
//   4. 用 cin / cout 时不需要手动 flush，main 正常 return 就会输出。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/00_basics.cpp -o /tmp/demo00
//   /tmp/demo00 < tests/samples/00_basics.in
// ============================================================

#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

int main() {
    int n = 0;
    if (!(cin >> n) || n <= 0) {
        return 0;  // 空输入：什么都不输出
    }

    vector<long long> numbers(n);
    for (int i = 0; i < n; ++i) {
        cin >> numbers[i];
    }

    long long sum = 0;
    long long maximum = numbers[0];  // 初值取第一个元素，不能取 0

    for (long long value : numbers) {
        sum += value;
        if (value > maximum) {
            maximum = value;
        }
    }

    cout << sum << " " << maximum << endl;
    return 0;
}
