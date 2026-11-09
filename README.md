# C++ 刷题仓库（面向算法基础薄弱、准备面试的同学）

一个"能跑、能看懂、能背下来"的 C++ 算法练习仓库。

- **算法清晰易懂**：变量名和函数名说人话（`left` / `right` / `currentDistance`），
  不炫技、不写一行流、不用竞赛黑话。
- **语法简单**：显式 `#include`、`std::` 前缀或少量 `using`、`vector` / `string` /
  `<algorithm>`；不用 `bits/stdc++.h`、不用 `#define` 宏、不用模板元编程。
- **中文注释齐全**：每个文件头部写清「这个算法是什么 / 思路 / 时间复杂度 /
  空间复杂度 / 常见易错点」，函数体和关键行也有注释，并且注释与代码一致。
- **每个文件都能单独编译运行**：自带 `main()`、示例输入输出和 `assert` 自检。

## 目录结构

```
.
├── README.md                       ← 你正在看的这个文件
├── src/                            ← 日常练习代码（默认看这里）
│   ├── 00_basics.cpp               C++ 刷题基本功：读入输出、防溢出
│   ├── 01_two_pointers.cpp         双指针（对撞指针）
│   ├── 02_sliding_window.cpp       滑动窗口（最长型 / 最短型）
│   ├── 03_binary_search.cpp        二分查找 / 二分答案 / 三分
│   ├── 04_hash_table.cpp           哈希表
│   ├── 05_linked_list.cpp          链表（哑节点 / 快慢指针 / 反转）
│   ├── 06_stack_queue.cpp          栈 / 队列 / 单调栈 / 单调队列
│   ├── 07_binary_tree.cpp          二叉树（遍历 / 层序 / BST / LCA）
│   ├── 08_backtracking.cpp         回溯（子集 / 组合 / 排列 / N 皇后）
│   ├── 09_graph.cpp                图（BFS / DFS / 拓扑 / 并查集 / Dijkstra）
│   ├── 10_heap.cpp                 堆 / 优先队列
│   ├── 11_dynamic_programming.cpp  动态规划（线性 / 背包 / 区间 / 双串）
│   ├── 12_greedy.cpp               贪心
│   ├── 13_string_kmp.cpp           字符串 / KMP
│   ├── 14_bit_operations.cpp       位运算
│   ├── 15_sorting.cpp              排序（快排 / 归并 / 快速选择 / 自定义排序）
│   ├── 16_prefix_diff.cpp          前缀和 / 差分
│   ├── 17_design.cpp               设计题（LRU / Trie / O(1) 随机集合）
│   ├── 18_advanced_structures.cpp  树状数组 / 线段树 / 离散化
│   └── 19_math.cpp                 数学基础（快速幂 / gcd / 组合数 / 质数筛）
├── tests/                          ← 样例输入输出 + 测试脚本
│   ├── samples/00_basics.in        样例输入（只有"从 stdin 读数据"的程序需要）
│   ├── expected/00_basics.out      对应的期望输出
│   └── run_tests.sh                编译（-Werror）+ 运行 + 对比输出
├── scripts/
│   └── build_all.sh                一键编译并运行全部练习代码
├── docs/
│   ├── cpp-interview-guide.md      知识库：按类别的原理 / 模板 / 必刷题 / 易错点
│   └── cpp-interview-guide.pdf     上面这份文档的 PDF 版本
└── template/                       ← 可选：机试用的竞赛模板（不是日常练习代码）
    ├── main.cpp                    快读快写单文件模板（保留原样，供机试使用）
    ├── simple_main.cpp             上面模板的"简单语法 + 中文注释"版本
    ├── snippets.md                 常用数据结构骨架（已被 src/ 里的简单版取代）
    └── README.md                   模板说明
```

## 环境要求

- 一个支持 C++17 的编译器。本仓库在 **Ubuntu 22.04 + g++ 11.4** 上验证通过；
  Clang、macOS 的 Apple Clang、Windows 上的 MinGW 也都可以。

```bash
g++ --version   # 确认 g++ 存在（apt install build-essential）
```

## 怎么编译、怎么运行

### 单个文件

每个 `.cpp` 都能独立编译、独立运行，例如：

```bash
g++ -std=c++17 -Wall -Wextra -O2 src/05_linked_list.cpp -o /tmp/demo
/tmp/demo
```

大部分文件不需要输入，直接运行就会跑完内置示例并打印结果；
`src/00_basics.cpp` 从标准输入读数据，用仓库里的样例跑：

```bash
g++ -std=c++17 -Wall -Wextra -O2 src/00_basics.cpp -o /tmp/demo00
/tmp/demo00 < tests/samples/00_basics.in
# 输出：15 5
```

### 一键跑全部（推荐）

```bash
./scripts/build_all.sh
```

这个脚本会：

1. 用 `g++ -std=c++17 -Wall -Wextra -O2` 编译 `src/` 下每个 `.cpp`，
   **出现 warning 也算失败**；
2. 运行每个程序，检查退出码（程序里的 `assert` 会自动校验示例结果）；
3. 对 `tests/samples/` 里有样例输入的程序，把输出和 `tests/expected/` 对比。

### 更严格的测试（提交前自查）

```bash
./tests/run_tests.sh
```

和上面一样，但多了 `-Werror`（警告直接当错误），并用 `diff` 显示具体差异，
最后输出 `PASS / FAIL` 汇总，方便贴到笔记或 PR 里。

## 学习路线（12 周，适合基础薄弱）

每周只攻一个专题，**每类先做 2 道最经典的题**，做完合上题解凭记忆手打一遍模板。
题号、原理、易错点都在 [`docs/cpp-interview-guide.md`](docs/cpp-interview-guide.md) 里。

| 周 | 专题 | 本仓库对应文件 | 每天 |
|---|---|---|---|
| 1 | 读入输出 / 双指针 / 滑动窗口 | `src/00` `src/01` `src/02` | 2 题 |
| 2 | 二分 + 哈希 | `src/03` `src/04` | 2 题 |
| 3 | 链表 + 栈 | `src/05` `src/06` | 2 题 |
| 4 | 二叉树（重点） | `src/07` | 3 题 |
| 5 | 回溯 / DFS | `src/08` | 2 题 |
| 6 | 图（BFS / 拓扑 / 并查集） | `src/09` | 2 题 |
| 7 | 堆 + 贪心 | `src/10` `src/12` | 2 题 |
| 8 | 动态规划（一）：线性 / 背包 | `src/11` | 2 题 |
| 9 | 动态规划（二）：区间 / 双串 | `src/11` | 2 题 |
| 10 | 字符串 + 位运算 + 排序 | `src/13` `src/14` `src/15` | 2 题 |
| 11 | 前缀和差分 + 设计题 | `src/16` `src/17` | 3 题 |
| 12 | 进阶结构 + 数学 + 错题重做 | `src/18` `src/19` | 复习 |

**每天固定 30 分钟**：合上所有资料，手打当天专题的模板骨架。写不出来就是没记住。

## 每个文件长什么样

以 `src/03_binary_search.cpp` 为例，结构是统一的：

```cpp
// ============================================================
// 专题 03：二分查找
//
// 适用题型：……
// 核心思想：……
// 时间复杂度 / 空间复杂度：……
// 易错点：……
// 编译运行：g++ -std=c++17 -Wall -Wextra -O2 src/03_binary_search.cpp -o /tmp/demo03
// ============================================================

#include <algorithm>
#include <vector>

// 函数上方写清「思路 / 时间复杂度 / 易错点」
int lowerBound(const std::vector<int>& numbers, int target) { ... }

int main() {
    // 每个示例都打印输入、输出，并用 assert 校验结果
    assert(...);
    std::cout << "03 二分查找：全部示例通过" << std::endl;
    return 0;
}
```

这样做的目的：**读代码时不用跳来跳去**，一个文件就是一个专题的完整笔记，
可以直接当"可运行的复习卡片"用。

## 关于 `template/`（可选）

`template/` 里保留了一份**竞赛风格**的单文件模板（`bits/stdc++.h`、宏、快读快写），
只适合机试时"卡输入输出速度"的场景。**日常练习请用 `src/` 里的简单写法**，
两者不要混用。`template/simple_main.cpp` 是同一份模板的简单语法 + 中文注释版本，
想看懂原模板在做什么可以对照着读。

## 约定

- 所有代码统一 `-std=c++17`，不依赖第三方库。
- 变量名用完整单词（`prefixSum` 而不是 `pre`，`currentDistance` 而不是 `d`）。
- 注释写"为什么"和"这一步在干什么"，不写废话。
- 每个文件末尾打印 `XX 全部示例通过`，方便测试脚本判断。
