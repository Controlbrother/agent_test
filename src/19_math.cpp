// ============================================================
// 专题 19：数学基础（快速幂 / 最大公约数 / 组合数 / 质数筛）
//
// 本节实现：快速幂、gcd / lcm、组合数 C(n, k) 取模、质数筛、试除法判质数。
//
// 同思路可解：约数个数（用质数筛出的质因子试除）、
//           扩展欧几里得求逆元（本节用费马小定理算逆元，前提是模数为质数）。
//
// 核心思想：
//   这些是"当工具用"的数学函数，面试时经常作为大题的一小部分出现。
//   把这里的 4 个函数背熟，绝大多数题目的数学部分就够用了。
//
// 时间复杂度 / 空间复杂度：每个函数单独标注。
//
// 易错点：
//   1. 快速幂的 result 初值写 1 % mod（mod 为 1 时结果应为 0）。
//   2. 取模运算里乘法可能溢出 int，变量统一用 long long。
//   3. 组合数取模要求模数是质数（本文件用 1e9+7），
//      非质数要用扩展欧几里得或 Lucas 定理，那是另一个话题。
//   4. 筛法里 j 从 i * i 开始，i * i 可能溢出 int。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/19_math.cpp -o /tmp/demo19
//   /tmp/demo19
// ============================================================

#include <cassert>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

const long long MOD = 1000000007LL;  // 1e9 + 7，面试最常用的模数

// ------------------------------------------------------------
// 1. 快速幂：求 base 的 exponent 次方，对 mod 取余
//
// 思路：
//   把指数写成二进制，例如 13 = 1101 = 8 + 4 + 1。
//   每次把底数平方（对应二进制下一位），
//   指数当前位是 1 就把当前底数乘进答案。
//
// 时间复杂度 O(log exponent)，空间复杂度 O(1)。
// ------------------------------------------------------------
long long fastPower(long long base, long long exponent, long long mod = MOD) {
    long long result = 1 % mod;  // 注意不是 1：mod == 1 时答案应为 0
    base %= mod;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = result * base % mod;
        }
        base = base * base % mod;
        exponent /= 2;
    }
    return result;
}

// ------------------------------------------------------------
// 2. 最大公约数（辗转相除 / 欧几里得算法）
//
// 思路：gcd(a, b) = gcd(b, a % b)，直到 b == 0，此时 a 就是答案。
// 最小公倍数 = |a| / gcd(|a|, |b|) * |b|（先除后乘，防止溢出；先取绝对值保证结果非负）。
//
// 时间复杂度 O(log min(a, b))，空间复杂度 O(1)。
// ------------------------------------------------------------
long long greatestCommonDivisor(long long a, long long b) {
    while (b != 0) {
        long long remainder = a % b;
        a = b;
        b = remainder;
    }
    return a < 0 ? -a : a;
}

// 最小公倍数：先取绝对值再算，保证结果非负
// （数学上 lcm 定义为非负；a / gcd * b 这个公式用来防溢出。
//   直接对 a、b 用公式时如果一正一负会得到负数，所以先取绝对值。）
long long leastCommonMultiple(long long a, long long b) {
    long long positiveA = a < 0 ? -a : a;
    long long positiveB = b < 0 ? -b : b;
    if (positiveA == 0 || positiveB == 0) return 0;
    return positiveA / greatestCommonDivisor(positiveA, positiveB) * positiveB;
}

// ------------------------------------------------------------
// 3. 组合数 C(n, k) 取模（预处理阶乘 + 逆元）
//
// 思路：
//   C(n, k) = n! / (k! * (n-k)!)。
//   在模质数意义下，除以 x 等于乘以 x 的逆元，
//   而 x 的逆元 = x^(mod - 2)（费马小定理）。
//
//   预处理出 fact[i] = i! % mod 和 invFact[i] = 1 / i! % mod，
//   之后每次查询 O(1)。
//
// 时间复杂度 预处理 O(n log mod)（其实一次快速幂 + O(n)），查询 O(1)
// 空间复杂度 O(n)
//
// 易错点：n 必须小于 mod，否则阶乘会变成 0（Lucas 定理才能处理更大范围）。
// ------------------------------------------------------------
class Combination {
public:
    explicit Combination(int maxN) {
        factorial.assign(maxN + 1, 1);
        inverseFactorial.assign(maxN + 1, 1);

        for (int i = 1; i <= maxN; ++i) {
            factorial[i] = factorial[i - 1] * i % MOD;
        }

        // 先算 maxN! 的逆元，再倒着推出每个 i! 的逆元
        inverseFactorial[maxN] = fastPower(factorial[maxN], MOD - 2, MOD);
        for (int i = maxN; i > 0; --i) {
            inverseFactorial[i - 1] = inverseFactorial[i] * i % MOD;
        }
    }

    long long choose(int n, int k) const {
        if (k < 0 || k > n) return 0;
        return factorial[n] * inverseFactorial[k] % MOD *
               inverseFactorial[n - k] % MOD;
    }

private:
    vector<long long> factorial;
    vector<long long> inverseFactorial;
};

// ------------------------------------------------------------
// 4. 埃拉托斯特尼筛法：求 [2, n] 内所有质数
//
// 思路：
//   从 2 开始，把每个质数的倍数全部标记成"合数"。
//   只枚举到 sqrt(n) 就够，因为更大的合数一定有小质因子。
//
// 时间复杂度 O(n log log n)，空间复杂度 O(n)。
//
// 易错点：内层循环从 i * i 开始（更小的倍数已经被更小的质数筛过了），
//   i * i 要用 long long 计算，i 接近 1e9 时会溢出 int。
// ------------------------------------------------------------
vector<int> sieveOfEratosthenes(int n) {
    vector<bool> isComposite(n + 1, false);
    vector<int> primes;

    for (long long i = 2; i <= n; ++i) {
        if (isComposite[i]) continue;
        primes.push_back((int)i);
        for (long long j = i * i; j <= n; j += i) {
            isComposite[j] = true;
        }
    }
    return primes;
}

// ------------------------------------------------------------
// 5. 试除法判断质数
//
// 思路：只需试除到 sqrt(n)，因为如果 n = a * b，
//   那么 a 和 b 中必有一个 <= sqrt(n)。
//
// 时间复杂度 O(sqrt(n))，空间复杂度 O(1)。
// ------------------------------------------------------------
bool isPrime(long long n) {
    if (n < 2) return false;
    for (long long divisor = 2; divisor * divisor <= n; ++divisor) {
        if (n % divisor == 0) return false;
    }
    return true;
}

int main() {
    // 示例 1：快速幂
    cout << "快速幂：2^10 = " << fastPower(2, 10)
         << "，3^13 mod 1e9+7 = " << fastPower(3, 13) << endl;
    assert(fastPower(2, 10) == 1024);
    assert(fastPower(3, 13) == 1594323);      // 3^13 = 1594323
    assert(fastPower(5, 0) == 1);             // 任何数的 0 次方是 1
    assert(fastPower(2, 10, 1) == 0);         // mod 为 1 时结果为 0
    // 2^100 mod 1e9+7 的期望值由 Python 独立算出：
    //   pow(2, 100, 1000000007) == 976371285
    assert(fastPower(2, 100) == 976371285);

    // 示例 2：gcd / lcm
    cout << "gcd(12, 18) = " << greatestCommonDivisor(12, 18)
         << "，lcm(4, 6) = " << leastCommonMultiple(4, 6) << endl;
    assert(greatestCommonDivisor(12, 18) == 6);
    assert(greatestCommonDivisor(-12, 18) == 6);  // 负数也能算
    assert(leastCommonMultiple(4, 6) == 12);
    assert(leastCommonMultiple(-4, 6) == 12);      // 负数结果也非负
    assert(leastCommonMultiple(0, 5) == 0);

    // 示例 3：组合数
    Combination combination(100);
    cout << "组合数：C(5, 2) = " << combination.choose(5, 2)
         << "，C(100, 50) mod 1e9+7 = " << combination.choose(100, 50) << endl;
    assert(combination.choose(5, 2) == 10);   // 5 选 2 = 10
    assert(combination.choose(5, 0) == 1);
    assert(combination.choose(5, 6) == 0);    // k > n 时是 0
    // C(100, 50) mod 1e9+7 的期望值由 Python 独立算出：
    //   math.comb(100, 50) % 1000000007 == 538992043
    assert(combination.choose(100, 50) == 538992043);
    // 对称性：C(n, k) == C(n, n-k)
    assert(combination.choose(20, 3) == combination.choose(20, 17));
    assert(combination.choose(20, 3) == 1140);   // 20*19*18/6

    // 示例 4：质数筛
    vector<int> primes = sieveOfEratosthenes(30);
    cout << "30 以内的质数：";
    for (int prime : primes) cout << prime << " ";
    cout << endl;
    assert((primes == vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}));

    // 示例 5：试除法判质数
    cout << "判断质数：17 -> " << (isPrime(17) ? "是" : "否")
         << "，18 -> " << (isPrime(18) ? "是" : "否")
         << "，1 -> " << (isPrime(1) ? "是" : "否") << endl;
    assert(isPrime(17) == true);
    assert(isPrime(18) == false);
    assert(isPrime(1) == false);
    assert(isPrime(2) == true);

    cout << "19 数学基础：全部示例通过" << endl;
    return 0;
}
