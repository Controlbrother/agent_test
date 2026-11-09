// ============================================================
// 专题 14：位运算
//
// 适用题型：只出现一次的数字、位 1 的个数、2 的幂、
//           汉明距离、比特位计数、不用加减乘除做加法。
//
// 必须记住的 6 个操作：
//   n & 1          取最低位（判断奇偶）
//   n >> 1         右移一位（相当于除以 2）
//   n & (n - 1)    消掉最低位的 1
//   n & (-n)       取出最低位的 1（lowbit）
//   n ^ n = 0      异或：相同为 0，不同为 1
//   n ^ 0 = n
//
// 时间复杂度：每个函数 O(位数) = O(1)（最多 32 / 64 次）
// 空间复杂度：O(1)
//
// 易错点：
//   1. 对负数做 >> 是"算术右移"（高位补 1），不是逻辑右移；
//      要逻辑右移请先转成 unsigned。
//   2. 判断 2 的幂之前必须先排除 n <= 0，
//      因为 n = 0 时 0 & (0-1) == 0 也会"成立"。
//   3. & 的优先级比 == 低，写 (n & 1) == 1 要加括号。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/14_bit_operations.cpp -o /tmp/demo14
//   /tmp/demo14
// ============================================================

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// ------------------------------------------------------------
// 1. 统计二进制中 1 的个数（LeetCode 191）
//
// 思路：
//   每次 n = n & (n - 1) 都会消掉最低位的那个 1，
//   消几次就说明有几个 1。比逐位检查更快（只循环 1 的个数次）。
//
// 时间复杂度 O(1 的个数)，空间复杂度 O(1)。
// ------------------------------------------------------------
int countOnes(unsigned int n) {
    int count = 0;
    while (n != 0) {
        n &= n - 1;  // 消掉最低位的 1
        ++count;
    }
    return count;
}

// ------------------------------------------------------------
// 2. 只出现一次的数字（LeetCode 136）
//
// 思路：
//   异或满足交换律和结合律，且 x ^ x = 0、x ^ 0 = x。
//   把全部数字异或起来，成对出现的都抵消成 0，
//   剩下的就是只出现一次的那个。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int singleNumber(const vector<int>& numbers) {
    int result = 0;
    for (int value : numbers) {
        result ^= value;
    }
    return result;
}

// ------------------------------------------------------------
// 3. 判断是不是 2 的幂（LeetCode 231）
//
// 思路：
//   2 的幂的二进制里只有一个 1，所以 n & (n - 1) == 0。
//   必须先排除 n <= 0（0 和负数都不是 2 的幂）。
//
// 时间复杂度 O(1)，空间复杂度 O(1)。
// ------------------------------------------------------------
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// ------------------------------------------------------------
// 4. 汉明距离（LeetCode 461）
//
// 思路：
//   两个数不同的二进制位，在异或结果里就是 1。
//   所以先异或，再统计 1 的个数。
//
// 时间复杂度 O(1)，空间复杂度 O(1)。
// ------------------------------------------------------------
int hammingDistance(int a, int b) {
    return countOnes((unsigned int)(a ^ b));
}

// ------------------------------------------------------------
// 5. 比特位计数（LeetCode 338）—— DP + lowbit
//
// 思路：
//   dp[i] = dp[i 去掉最低位的 1] + 1，即 dp[i] = dp[i & (i-1)] + 1。
//   因为 i & (i-1) 比 i 小，所以从小到大递推即可。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<int> countBits(int n) {
    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        dp[i] = dp[i & (i - 1)] + 1;  // 去掉最低位的 1，再加 1
    }
    return dp;
}

// ------------------------------------------------------------
// 6. 不用加减乘除做加法（LeetCode 371 / 剑指 Offer 65）
//
// 思路：
//   不考虑进位的结果 = a ^ b；
//   只考虑进位的结果 = (a & b) << 1。
//   两者相加，直到进位变成 0。
//
// 注意：C++ 里对有符号负数左移是未定义行为，
//   所以这里统一转成 unsigned 计算，最后再转回来。
//
// 时间复杂度 O(1)（最多 32 轮），空间复杂度 O(1)。
// ------------------------------------------------------------
int addWithoutOperator(int a, int b) {
    unsigned int x = (unsigned int)a;
    unsigned int y = (unsigned int)b;
    while (y != 0) {
        unsigned int carry = (x & y) << 1;  // 进位
        x = x ^ y;                          // 不带进位的和
        y = carry;
    }
    return (int)x;
}

// ------------------------------------------------------------
// 7. 交换两个数（不用临时变量）—— 异或技巧
//
// 注意：仅用于演示。工程代码不要这样写：
//   可读性差，而且 a 和 b 是同一个变量时会把值清零。
//
// 时间复杂度 O(1)，空间复杂度 O(1)。
// ------------------------------------------------------------
void swapWithXor(int& a, int& b) {
    a ^= b;
    b ^= a;
    a ^= b;
}

// 把整数打印成 8 位二进制，方便观察位运算效果
void printBinary(int value) {
    for (int bit = 7; bit >= 0; --bit) {
        cout << ((value >> bit) & 1);
    }
}

int main() {
    // 示例 1：统计 1 的个数
    cout << "数字 11 的二进制：";
    printBinary(11);
    cout << "，1 的个数 = " << countOnes(11) << endl;
    assert(countOnes(11) == 3);    // 1011
    assert(countOnes(0) == 0);
    assert(countOnes(4294967295U) == 32);  // 全 1

    // 示例 2：只出现一次的数字
    vector<int> numbers = {4, 1, 2, 1, 2};
    cout << "只出现一次的数字：[4, 1, 2, 1, 2] -> " << singleNumber(numbers)
         << endl;
    assert(singleNumber(numbers) == 4);

    // 示例 3：判断 2 的幂
    cout << "判断 2 的幂：1 -> " << (isPowerOfTwo(1) ? "是" : "否")
         << "，16 -> " << (isPowerOfTwo(16) ? "是" : "否")
         << "，0 -> " << (isPowerOfTwo(0) ? "是" : "否")
         << "，-16 -> " << (isPowerOfTwo(-16) ? "是" : "否") << endl;
    assert(isPowerOfTwo(1) == true);
    assert(isPowerOfTwo(16) == true);
    assert(isPowerOfTwo(0) == false);
    assert(isPowerOfTwo(-16) == false);

    // 示例 4：汉明距离
    cout << "汉明距离：1 和 4 -> " << hammingDistance(1, 4) << endl;
    assert(hammingDistance(1, 4) == 2);  // 001 和 100，两位不同

    // 示例 5：比特位计数
    vector<int> bits = countBits(5);
    cout << "0..5 每个数的 1 的个数：";
    for (int value : bits) cout << value << " ";
    cout << endl;
    assert((bits == vector<int>{0, 1, 1, 2, 1, 2}));

    // 示例 6：不用加减乘除做加法
    cout << "不用 + 做加法：3 + 5 = " << addWithoutOperator(3, 5)
         << "，-1 + 2 = " << addWithoutOperator(-1, 2) << endl;
    assert(addWithoutOperator(3, 5) == 8);
    assert(addWithoutOperator(-1, 2) == 1);
    assert(addWithoutOperator(-5, -7) == -12);

    // 示例 7：异或交换
    int a = 3;
    int b = 8;
    swapWithXor(a, b);
    cout << "异或交换：a = " << a << "，b = " << b << endl;
    assert(a == 8 && b == 3);

    cout << "14 位运算：全部示例通过" << endl;
    return 0;
}
