# 常用工具：DSU / 线段树 / 树状数组 / Dijkstra / 并查集等骨架
# 说明：这里只放模板骨架，做题时按需复制到 solution.cpp 里，不要直接 include
# （大多数评测机只编译单个文件）。

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
