# 可选：机试用的竞赛模板（日常练习请用 `../src/`）

这个目录放的是**竞赛 / 机试风格**的单文件模板，和仓库根目录 `src/` 里的
日常练习代码是**两套东西**，不要混用：

| 目录 | 定位 | 风格 |
|---|---|---|
| `src/` | 日常刷题、复习、面试手写 | 显式 `#include`、`std::`、`cin/cout`、完整中文注释 |
| `template/` | 机试 / 在线评测的最后 5 分钟起手文件 | `bits/stdc++.h`、宏、手写快读快写 |

如果你只是准备面试、想先把算法写清楚，**完全不需要看这个目录**。

## 文件

| 文件 | 用途 |
|---|---|
| `main.cpp` | 竞赛模板原版：`bits/stdc++.h`、快读快写、`dbg` / `CHECK` 调试宏。需要 `-std=c++20` |
| `simple_main.cpp` | 上面模板的**简单语法 + 中文注释**版本：显式 `#include`、`cin/cout`、无宏，`-std=c++17` 即可编译 |
| `snippets.md` | 常用数据结构 / 算法骨架速查（二分、DSU、BIT、线段树、Dijkstra、离散化、组合数）。**注意**：这些骨架已被 `../src/` 里带完整注释的版本取代，这里保留是为了机试时快速复制 |
| `sample_in.txt` / `sample_out.txt` | 两个模板共用的样例输入输出 |

## 快速开始

```bash
# 竞赛模板（C++20）
g++ -std=c++20 -O2 -o main template/main.cpp
./main < template/sample_in.txt      # 输出：15 5

# 简单语法版（C++17，推荐先读这个）
g++ -std=c++17 -Wall -Wextra -O2 -o simple template/simple_main.cpp
./simple < template/sample_in.txt    # 输出：15 5

# Debug 编译：开 CHECK、计时、dbg()
g++ -std=c++20 -O0 -g -DLOCAL -o main template/main.cpp
```

## 竞赛模板里有什么

**快速 IO**：`readInt` / `readStr` / `writeInt` / `writeStr` / `flush`，64KB 缓冲，
比默认的 `cin` 快。读完必须 `flush()`（`main` 里已经调用了）。

```cpp
int n; readInt(n);
vector<int> a(n);
for (int& x : a) readInt(x);
writeInt(n); writeStr("\n");
flush();
```

**调试**（加 `-DLOCAL` 才生效，Release 下全部编译为空）：
- `dbg(x, y, z)`：把变量名和值打到 stderr
- `CHECK(cond)`：条件不满足时打印行号和表达式
- 作用域结束时自动打印耗时 `time: N ms`

**常用工具**：`chmin` / `chmax`（返回是否更新）、`qpow` 快速幂、`gcdll`、
`lowerBoundLL` 整数二分、常量 `INF` / `LINF` / `MOD`。

**多测**：`main()` 里默认 `int T = 1`；需要多组输入时改成 `readInt(T)`。

## 注意

1. 模板默认从 stdin 读、写 stdout，`main()` 最后一定 `flush()`，
   否则缓冲区里的输出会丢。
2. `solve()` 里对空输入做了保护（`n <= 0` 直接 return），避免输出脏数据。
3. `snippets.md` 里的是**骨架**，做题时复制到单文件里，不要 `#include` 它
   —— 多数评测机只编译一个文件。
4. `MOD` 默认 `1'000'000'007`，题目要求 `998'244'353` 时记得改。

## 验证记录

在 Ubuntu 22.04 / g++ 11.4.0 上：

- `template/main.cpp` 以 `-std=c++20 -O2` 编译无 warning
- `template/simple_main.cpp` 以 `-std=c++17 -Wall -Wextra -Werror -O2` 编译无 warning
- 两者跑 `template/sample_in.txt`（`5 / 1 2 3 4 5`）都输出 `15 5`
- `src/` 里对应的简单版实现由 `../scripts/build_all.sh` 和
  `../tests/run_tests.sh` 统一编译运行，20 个文件全部通过

## pnpm 安装说明

[pnpm](https://pnpm.io/) 是基于内容寻址存储的 Node.js 包管理器。本仓库目前没有
`package.json`，无需安装依赖；若后续引入 Node.js 项目，可按以下步骤安装 pnpm。

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
