# 常用工具：DSU / 线段树 / 树状数组 / Dijkstra / 并查集等骨架（机试速查）

> **定位**：这里是**竞赛/机试风格的速查骨架**，命名短、注释少，适合考试时直接复制。
> 如果你在学算法、准备面试手写，请先看 `src/` 里对应的**简单语法 + 完整中文注释**版本
> （每个文件都能独立编译运行，带示例和自检）：
>
> | 本文骨架 | 推荐阅读的练习代码 |
> |---|---|
> | 并查集 | [`../src/09_graph.cpp`](../src/09_graph.cpp) 里的 `DisjointSet` |
> | 树状数组 | [`../src/18_advanced_structures.cpp`](../src/18_advanced_structures.cpp) 里的 `FenwickTree` |
> | 线段树（区间加 / 区间和） | [`../src/18_advanced_structures.cpp`](../src/18_advanced_structures.cpp) 里的 `SegmentTree` |
> | Dijkstra | [`../src/09_graph.cpp`](../src/09_graph.cpp) 里的 `dijkstra` |
> | 单调栈 | [`../src/06_stack_queue.cpp`](../src/06_stack_queue.cpp) |
> | 离散化 | [`../src/18_advanced_structures.cpp`](../src/18_advanced_structures.cpp) 里的 `discretize` |
> | 二分法（整数 / 答案 / 实数 / 三分 / 旋转数组） | [`../src/03_binary_search.cpp`](../src/03_binary_search.cpp) |
> | 组合数预处理 | [`../src/19_math.cpp`](../src/19_math.cpp) 里的 `Combination` |
>
> 说明：这里只放模板骨架，做题时按需复制到 solution.cpp 里，不要直接 include
> （大多数评测机只编译单个文件）。

## 并查集（带路径压缩 + 按秩合并）
```cpp
struct DSU {
    vector<int> p, r;
    explicit DSU(int n) : p(n), r(n, 0) { iota(all(p), 0); }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) ++r[a];
        return true;
    }
    bool same(int a, int b) { return find(a) == find(b); }
};
```

## 树状数组（单点加 / 前缀和）
```cpp
template <class T>
struct BIT {
    int n;
    vector<T> t;
    explicit BIT(int n) : n(n), t(n + 1, T{}) {}
    void add(int i, T v) {                    // i 从 1 开始
        for (; i <= n; i += i & -i) t[i] += v;
    }
    T sum(int i) const {                      // [1, i]
        T s{};
        for (; i > 0; i -= i & -i) s += t[i];
        return s;
    }
    T range(int l, int r) const { return sum(r) - sum(l - 1); }
};
```

## 线段树（区间加 / 区间和）
```cpp
struct SegTree {
    int n;
    vector<ll> sum, lazy;
    explicit SegTree(int n) : n(n), sum(4 * n, 0), lazy(4 * n, 0) {}
    void apply(int i, int l, int r, ll v) {
        sum[i] += v * (r - l + 1);
        lazy[i] += v;
    }
    void push(int i, int l, int r) {
        if (lazy[i] == 0 || l == r) return;
        int m = (l + r) >> 1;
        apply(i << 1, l, m, lazy[i]);
        apply(i << 1 | 1, m + 1, r, lazy[i]);
        lazy[i] = 0;
    }
    void add(int i, int l, int r, int ql, int qr, ll v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) return apply(i, l, r, v);
        push(i, l, r);
        int m = (l + r) >> 1;
        add(i << 1, l, m, ql, qr, v);
        add(i << 1 | 1, m + 1, r, ql, qr, v);
        sum[i] = sum[i << 1] + sum[i << 1 | 1];
    }
    ll query(int i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return sum[i];
        push(i, l, r);
        int m = (l + r) >> 1;
        return query(i << 1, l, m, ql, qr) + query(i << 1 | 1, m + 1, r, ql, qr);
    }
    void add(int l, int r, ll v) { add(1, 0, n - 1, l, r, v); }
    ll query(int l, int r) { return query(1, 0, n - 1, l, r); }
};
```

## Dijkstra（邻接表，非负权）
```cpp
vector<ll> dijkstra(const vector<vector<pll>>& g, int src) {
    vector<ll> dist(g.size(), LINF);
    priority_queue<pll, vector<pll>, greater<pll>> pq;
    dist[src] = 0;
    pq.emplace(0, src);
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;
        for (auto [v, w] : g[u])
            if (chmin(dist[v], d + w)) pq.emplace(dist[v], v);
    }
    return dist;
}
```

## 单调栈：下一个更大元素
```cpp
vi nextGreater(const vi& a) {
    int n = sz(a);
    vi res(n, -1);
    vector<int> st;
    for (int i = n - 1; i >= 0; --i) {
        while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
        if (!st.empty()) res[i] = st.back();
        st.push_back(i);
    }
    return res;
}
```

## 离散化
```cpp
vi compress(vi a) {              // 返回每个元素的名次（从 0 开始）
    vi b = a;
    sort(all(b));
    b.erase(unique(all(b)), b.end());
    for (int& x : a) x = int(lower_bound(all(b), x) - b.begin());
    return a;
}
```

## 二分法

### 1. 整数二分：只用这一个函数，不要背四种
```cpp
// 在 [lo, hi) 里找第一个满足 pred(x) 为 true 的位置。
// pred 必须在区间内单调（false...false true...true），否则结果无意义。
// 找不到时返回 hi —— 调用方自己判断 lo <= res < hi。
template <class F>
ll lowerBoundLL(ll lo, ll hi, F pred) {
    while (lo < hi) {
        ll mid = lo + (hi - lo) / 2;   // 不写 (lo+hi)/2：防 lo+hi 溢出
        if (pred(mid))
            hi = mid;
        else
            lo = mid + 1;
    }
    return lo;
}
```
**四种经典写法都能用它表达**（`a` 升序）：

| 需求 | 写法 |
|---|---|
| 第一个 `>= x` | `lowerBoundLL(0, n, [&](ll i){ return a[i] >= x; })` |
| 第一个 `> x` | `lowerBoundLL(0, n, [&](ll i){ return a[i] > x; })` |
| 最后一个 `<= x` | `lowerBoundLL(0, n, [&](ll i){ return a[i] > x; }) - 1` |
| 最后一个 `< x` | `lowerBoundLL(0, n, [&](ll i){ return a[i] >= x; }) - 1` |

不要写 `while (lo <= hi)` 那种版本：`mid ± 1` 漏一处就是死循环，而这个版本区间收缩永远严格。

### 2. STL：能用就直接用
```cpp
// 升序 a：返回迭代器，都 O(log n)
auto it1 = lower_bound(all(a), x);        // 第一个 >= x
auto it2 = upper_bound(all(a), x);        // 第一个 > x
bool has = binary_search(all(a), x);      // 存在性
// 下标版：int pos = int(it - a.begin());  pos == sz(a) 表示没找到

// 降序数组（greater<int>）要传比较器，方向反过来：
auto it3 = lower_bound(a.begin(), a.end(), x, greater<int>()); // 第一个 <= x
```

**易错**：对 `vector<ll>` 查找时 `x` 也用 `ll`，别传 `int` 字面量给 `lower_bound` 的模板推导添乱；`lower_bound` 要求区间**有序**，且比较器与排序时用的必须一致。

### 3. 二分答案（"最小的最大值 / 最大的最小值"）
```cpp
// 求最小的 t 使 check(t) 成立；check 必须满足：t 不成立 => 更小的 t 也不成立
ll binarySearchAnswer(ll lo, ll hi, F check) {   // 答案闭区间 [lo, hi]
    ll l = lo, r = hi;
    while (l < r) {
        ll mid = l + (r - l) / 2;
        if (check(mid))
            r = mid;      // mid 可行 -> 答案在 [l, mid]
        else
            l = mid + 1;  // mid 不可行 -> 答案在 [mid+1, r]
    }
    return l;
}
```
判 `check` 时的典型错误：**把"答案可行"和"答案更大/更小"搞反**。记住口诀——`r = mid` 对应"存在更小的可行解"，`l = mid + 1` 对应"mid 不满足"。

### 4. 实数二分（精度版，比固定 100 次迭代更可控）
```cpp
// 求 f(x) = 0 的根 / 极值，单调区间 [lo, hi]
double binarySearchReal(double lo, double hi, F pred, int iterations = 200) {
    for (int i = 0; i < iterations; ++i) {
        double mid = (lo + hi) / 2;
        if (pred(mid))
            hi = mid;
        else
            lo = mid;
    }
    return (lo + hi) / 2;
}
```
固定迭代 200 次一定收敛（区间每次减半，double 只有 53 位有效位），比 `while (hi - lo > 1e-9)` 更稳——后者在 `hi`、`lo` 很大时可能永远达不到。

### 5. 三分求单峰极值（凸/凹函数）
```cpp
// 求 [lo, hi] 上单峰函数 f 的最小值（f 先减后增）；求最大就把比较反一下
double ternarySearch(double lo, double hi) {
    for (int i = 0; i < 200; ++i) {
        double m1 = lo + (hi - lo) / 3;
        double m2 = hi - (hi - lo) / 3;
        if (f(m1) < f(m2))
            hi = m2;   // 极小值在 [lo, m2]
        else
            lo = m1;
    }
    return f((lo + hi) / 2);
}
```

### 6. 两类"非数组"二分，容易忘
```cpp
// (a) 二分答案的范围常常不是数组下标，而是数值域：
//     如"最少要多少天"、"最大载重" -> lo=下界, hi=上界, 判 check(mid)

// (b) 旋转数组找最小值（无重复元素）：
int findMin(vector<int>& a) {
    int lo = 0, hi = sz(a) - 1;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] > a[hi]) lo = mid + 1;   // 最小值在右半
        else hi = mid;                       // 最小值在左半（含 mid）
    }
    return a[lo];
}
```

### 7. 边界自查清单（写完必过一遍）
- `lo`、`hi` 是**闭区间**还是**半开区间**？全程统一，别混。
- `hi` 初值取 `n` 还是 `n - 1`？半开写法取 `n`（`a[n]` 不访问，只是哨兵位）。
- 返回 `lo` 后，`lo == hi` 表示**没找到**——调用方有没有判？
- `mid` 用 `lo + (hi - lo) / 2`，不用 `(lo + hi) / 2`（`lo+hi` 溢出是经典 WA）。
- 答案可能是负数时，`lo` 初值别写成 `0`。
- `check(mid)` 里的 `mid` 会不会越界访问数组？

## 组合数预处理（模素数）
```cpp
vector<ll> fact, inv_fact;
void initComb(int n, ll mod = MOD) {
    fact.assign(n + 1, 1);
    inv_fact.assign(n + 1, 1);
    for (int i = 1; i <= n; ++i) fact[i] = fact[i - 1] * i % mod;
    inv_fact[n] = qpow(fact[n], mod - 2, mod);
    for (int i = n; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i % mod;
}
ll C(int n, int k, ll mod = MOD) {
    if (k < 0 || k > n) return 0;
    return fact[n] * inv_fact[k] % mod * inv_fact[n - k] % mod;
}
```
