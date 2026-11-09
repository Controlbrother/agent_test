# C++ 刷题模板

单文件竞赛/刷题模板，C++20，无第三方依赖。改 `solve()` 即可用。

## 文件

| 文件 | 用途 |
|---|---|
| `template/main.cpp` | 主模板：快速 IO、常量、常用函数、调试宏、解题区 |
| `template/snippets.md` | 常用数据结构/算法骨架（二分、DSU、BIT、线段树、Dijkstra、离散化、组合数…） |

## 快速开始

```bash
# Release 编译（评测用）
g++ -std=c++20 -O2 -pipe -o main template/main.cpp

# Debug 编译（本地调试：开 assert 风格的 CHECK、计时、dbg()）
g++ -std=c++20 -O0 -g -DLOCAL -o main template/main.cpp

# 跑样例
./main < in.txt
```

## 模板里有什么

**快速 IO**：`readInt` / `readStr` / `writeInt` / `writeStr` / `flush`，6 万级缓冲，比 `cin` 快，且不受 `sync_with_stdio` 影响。读完必须 `flush()`（`main` 已调用）。

```cpp
int n; readInt(n);
vector<int> a(n);
for (int& x : a) readInt(x);
writeInt(n); writeStr("\n");
flush();
```

**调试**（加 `-DLOCAL` 生效，Release 下全部编译为空）：
- `dbg(x, y, z)` — 打变量名和值到 stderr
- `CHECK(cond)` — 条件不满足时打印行号和表达式
- 作用域结束时自动打印耗时 `time: N ms`

**常用工具**：
- `chmin` / `chmax` 返回是否更新（可直接用于 `if (chmin(a,b)) ...`）
- `qpow` 快速幂、`gcdll`、`lowerBoundLL` 整数二分
- 常量 `INF` / `LINF` / `MOD`

**多测**：`main()` 里默认 `int T = 1`；需要多组输入时改成 `readInt(T)`。

## 注意

1. 模板默认从 stdin 读、写 stdout，`main()` 最后一定 `flush()`，否则缓冲区里的输出会丢。
2. `solve()` 里对空输入做了保护（`n <= 0` 直接 return），避免输出脏数据。
3. `snippets.md` 里的是**骨架**，做题时复制到单文件里，不要 `#include` 它 —— 多数评测机只编译一个文件。
4. `MOD` 默认 `1'000'000'007`，题目要求 `998'244'353` 时记得改（NTT 类题必须改）。

## 已验证

在 Ubuntu 24.04 / g++ 13.3.0 上：

- `template/main.cpp` 以 `-std=c++20 -O2` 和 `-O0 -g -DLOCAL` 两种方式编译均无 warning（`-Wall -Wextra` 下也无）
- 样例：`5 / 1 2 3 4 5` → `15 5`；`3 / -5 0 -2` → `-7 0`；全 `1e9` 四元素 → `4000000000 1000000000`（不溢出，走 `long long`）
- 空输入、前后空白、制表符混排均正常
- `snippets.md` 内全部骨架抽出来编译并跑断言，二分（整数/答案/实数/旋转数组）、DSU / BIT / 线段树（区间加区间和）/ Dijkstra / 单调栈 / 离散化 / 组合数全部通过
- 二分部分额外做了溢出与边界回归：`lowerBoundLL` 与 STL `lower_bound`/`upper_bound` 在 `0..10` 全取值逐一比对一致；`lo=0, hi=4e9` 不溢出；旋转数组含单元素、已排序、断点在首尾等形态

## pnpm 安装说明

[pnpm](https://pnpm.io/) 是基于内容寻址存储的 Node.js 包管理器。本仓库目前没有 `package.json`，无需安装依赖；若后续引入 Node.js 项目，可按以下步骤安装 pnpm。

推荐通过 Node.js 自带的 Corepack 启用（需要 Node.js 16.13 及以上版本）：

```bash
corepack enable
corepack prepare pnpm@latest --activate
pnpm -v
```

也可以用 npm 全局安装：

```bash
npm install -g pnpm
pnpm -v
```

安装完成后，在含 `package.json` 的项目目录中执行 `pnpm install` 即可安装依赖。
