// 刷题模板（单文件版）· C++20
//
// 编译：
//   g++ -std=c++20 -O2 -pipe -o main main.cpp
// 调试：
//   g++ -std=c++20 -O0 -g -DLOCAL -o main main.cpp
//
// 做题时只需要改 solve() 里面的内容。LOCAL 打开时：宽限 assert、
// 输出耗时到 stderr、可直接用 dbg() 打变量。
#include <bits/stdc++.h>
using namespace std;

// ---------- 基本别名 ----------
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vi = vector<int>;
using vll = vector<ll>;

#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define pb push_back
#define eb emplace_back
#define F first
#define S second

// ---------- 常量 ----------
constexpr int INF = 0x3f3f3f3f;
constexpr ll LINF = 0x3f3f3f3f3f3f3f3fLL;
constexpr int MOD = 1'000'000'007;  // 998'244'353 按题意改

// ---------- 快速 IO ----------
namespace fastio {
constexpr int BUFSZ = 1 << 16;
char ibuf[BUFSZ], obuf[BUFSZ];
int ipos = 0, ilen = 0, opos = 0;

inline char readChar() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUFSZ, stdin);
        ipos = 0;
        if (ilen == 0) return 0;
    }
    return ibuf[ipos++];
}

inline void flush() {
    if (opos) fwrite(obuf, 1, opos, stdout), opos = 0;
}

inline void pushChar(char c) {
    if (opos == BUFSZ) flush();
    obuf[opos++] = c;
}

inline void writeStr(const string& s) {
    for (char c : s) pushChar(c);
}

// 读整数：自动跳过所有非数字（含负数、空白）
template <class T>
bool readInt(T& out) {
    char c = readChar();
    while (c && (c < '0' || c > '9') && c != '-') c = readChar();
    if (!c) return false;
    bool neg = false;
    if (c == '-') neg = true, c = readChar();
    T val = 0;
    while (c >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        c = readChar();
    }
    out = neg ? -val : val;
    return true;
}

// 读字符串（不含空白），返回 false 表示 EOF
inline bool readStr(string& out) {
    char c = readChar();
    while (c && isspace((unsigned char)c)) c = readChar();
    if (!c) return false;
    out.clear();
    while (c && !isspace((unsigned char)c)) {
        out.push_back(c);
        c = readChar();
    }
    return true;
}

template <class T>
void writeInt(T val) {
    if (val < 0) pushChar('-'), val = -val;
    char tmp[24];
    int n = 0;
    do {
        tmp[n++] = char('0' + (int)(val % 10));
        val /= 10;
    } while (val);
    while (n) pushChar(tmp[--n]);
}
}  // namespace fastio

// 常用简写
using fastio::flush;
using fastio::readInt;
using fastio::readStr;
using fastio::writeInt;
using fastio::writeStr;

// 读一个值
template <class T>
T read() {
    T v{};
    readInt(v);
    return v;
}

// 输出一个值 + 换行
template <class T>
void println(const T& v) {
    if constexpr (is_integral_v<T>) {
        writeInt(v);
    } else if constexpr (is_same_v<T, string>) {
        writeStr(v);
    } else {
        writeStr(to_string(v));
    }
    writeStr("\n");
}

// ---------- 常用函数 ----------
template <class T>
bool chmin(T& a, const T& b) {
    return b < a ? (a = b, true) : false;
}
template <class T>
bool chmax(T& a, const T& b) {
    return b > a ? (a = b, true) : false;
}

ll qpow(ll a, ll e, ll mod = MOD) {
    ll r = 1 % mod;
    a %= mod;
    while (e) {
        if (e & 1) r = r * a % mod;
        a = a * a % mod;
        e >>= 1;
    }
    return r;
}

ll gcdll(ll a, ll b) { return b ? gcdll(b, a % b) : llabs(a); }

// 整数二分：返回第一个满足 pred 的位置，pred 需单调 false..false true..true
template <class F>
ll lowerBoundLL(ll lo, ll hi, F pred) {  // 区间 [lo, hi)
    while (lo < hi) {
        ll mid = lo + (hi - lo) / 2;
        if (pred(mid))
            hi = mid;
        else
            lo = mid + 1;
    }
    return lo;
}

// ---------- 调试 ----------
#ifdef LOCAL
#define dbg(...)                                                       \
    do {                                                               \
        cerr << "[" << __LINE__ << "] " << #__VA_ARGS__ << " = ";      \
        debug_out(__VA_ARGS__);                                        \
    } while (0)

template <class T>
void debug_out(const T& x) {
    if constexpr (is_same_v<T, string>) {
        cerr << '"' << x << '"';
    } else if constexpr (is_same_v<T, char>) {
        cerr << '\'' << x << '\'';
    } else {
        cerr << x;
    }
    cerr << '\n';
}

template <class T, class... Rest>
void debug_out(const T& x, const Rest&... rest) {
    if constexpr (is_same_v<T, string>) {
        cerr << '"' << x << '"';
    } else if constexpr (is_same_v<T, char>) {
        cerr << '\'' << x << '\'';
    } else {
        cerr << x;
    }
    cerr << ", ";
    debug_out(rest...);
}

#define CHECK(cond)                                                                     \
    do {                                                                                \
        if (!(cond)) {                                                                  \
            cerr << "CHECK failed at line " << __LINE__ << ": " << #cond << '\n';       \
        }                                                                               \
    } while (0)

struct Timer {
    chrono::steady_clock::time_point t0 = chrono::steady_clock::now();
    ~Timer() {
        auto ms = chrono::duration_cast<chrono::milliseconds>(
                      chrono::steady_clock::now() - t0)
                      .count();
        cerr << "time: " << ms << " ms\n";
    }
};
#else
#define dbg(...) ((void)0)
#define CHECK(cond) ((void)0)
#endif

// ---------- 解题区 ----------
void solve() {
    int n = 0;
    if (!readInt(n) || n <= 0) return;  // 空输入直接退出，不要输出脏数据
    vi a(n);
    for (int& x : a) readInt(x);

    // 示例：读 n 个数，输出与最大值
    ll sum = 0;
    int mx = -INF;
    for (int x : a) sum += x, chmax(mx, x);

    writeInt(sum);
    writeStr(" ");
    writeInt(mx);
    writeStr("\n");
}

int main() {
#ifdef LOCAL
    Timer _timer;
#endif
    // 多测：需要时把 1 改成 readInt(T)，循环 T 次
    int T = 1;
    // readInt(T);
    while (T--) {
        solve();
        // 多测之间如果需要清空全局变量，在这里做
    }
    flush();
    return 0;
}
