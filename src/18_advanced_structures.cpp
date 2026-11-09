// ============================================================
// 专题 18：树状数组 / 线段树 / 离散化
//
// 这三个是"进阶但面试偶尔会问"的工具，原本写在 template/snippets.md 里，
// 这里改写成简单、带完整注释、可直接运行的版本。
//
// 适用场景：
//   - 树状数组（BIT）：单点修改 + 前缀和查询，代码短、常数小；
//   - 线段树：区间修改 + 区间查询（这里演示"区间加 + 区间和"）；
//   - 离散化：数值很大但个数不多时，把大数值映射成 1..n 的小下标，
//     这样才能放进树状数组 / 线段树。
//
// 时间复杂度：
//   树状数组 每次操作 O(log n)
//   线段树   每次操作 O(log n)
//   离散化   O(n log n)（排序 + 二分）
// 空间复杂度：O(n) / O(4n)
//
// 易错点：
//   1. 树状数组的下标必须从 1 开始，0 会导致死循环（i += i & -i 里 i = 0）。
//   2. 线段树的数组要开 4 倍大小；lazy 标记下传前忘了清空会算重。
//   3. 离散化要先去重再二分，否则下标不连续。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/18_advanced_structures.cpp -o /tmp/demo18
//   /tmp/demo18
// ============================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// ------------------------------------------------------------
// 一、树状数组（Binary Indexed Tree）
//
// 用途：单点修改 + 前缀和查询，两者都是 O(log n)。
//
// 原理：
//   把下标 i 的二进制最低位的 1 对应的值记作 lowbit(i) = i & (-i)。
//   tree[i] 负责维护"从 i - lowbit(i) + 1 到 i"这一段的区间和。
//   修改时 i 不断加上 lowbit(i)，一路向上更新；
//   查询时 i 不断减去 lowbit(i)，一路向左累加。
//
// 注意：下标从 1 开始，tree 数组开 n + 1 位。
// ------------------------------------------------------------
class FenwickTree {
public:
    explicit FenwickTree(int n) : size_(n), tree_(n + 1, 0) {}

    // 第 index 个位置加 delta（index 从 1 开始）
    void add(int index, long long delta) {
        for (int i = index; i <= size_; i += i & (-i)) {
            tree_[i] += delta;
        }
    }

    // 前 index 个元素的和（1 + 2 + ... + index）
    long long prefixSum(int index) const {
        long long sum = 0;
        for (int i = index; i > 0; i -= i & (-i)) {
            sum += tree_[i];
        }
        return sum;
    }

    // 区间 [left, right] 的和（都从 1 开始）
    long long rangeSum(int left, int right) const {
        return prefixSum(right) - prefixSum(left - 1);
    }

private:
    int size_;
    vector<long long> tree_;
};

// ------------------------------------------------------------
// 二、线段树（区间加 + 区间和）
//
// 原理：
//   把区间建成一棵二叉树，每个节点维护一段区间的和。
//   "区间加"用 lazy 标记：整段被覆盖时只记在节点上，
//   等下次真正需要访问子节点时再下传（push down）。
//
// 节点 i 负责 [left, right]：
//   左孩子 2*i 负责 [left, middle]，右孩子 2*i+1 负责 [middle+1, right]。
//
// 注意：数组要开 4 倍，因为递归建树的下标不是紧密排列的。
// ------------------------------------------------------------
class SegmentTree {
public:
    explicit SegmentTree(const vector<long long>& numbers)
        : size_((int)numbers.size()) {
        sum_.assign(4 * size_ + 4, 0);
        lazy_.assign(4 * size_ + 4, 0);
        if (size_ > 0) {
            build(1, 0, size_ - 1, numbers);
        }
    }

    // 区间 [left, right] 全部加 value
    void rangeAdd(int left, int right, long long value) {
        if (size_ == 0) return;
        rangeAdd(1, 0, size_ - 1, left, right, value);
    }

    // 区间 [left, right] 的和
    long long rangeSum(int left, int right) {
        if (size_ == 0) return 0;
        return rangeSum(1, 0, size_ - 1, left, right);
    }

private:
    void build(int node, int left, int right, const vector<long long>& numbers) {
        if (left == right) {
            sum_[node] = numbers[left];
            return;
        }
        int middle = left + (right - left) / 2;
        build(node * 2, left, middle, numbers);
        build(node * 2 + 1, middle + 1, right, numbers);
        sum_[node] = sum_[node * 2] + sum_[node * 2 + 1];
    }

    // 把 lazy 标记下传给两个孩子
    void pushDown(int node, int left, int right) {
        if (lazy_[node] == 0 || left == right) return;

        int middle = left + (right - left) / 2;
        long long value = lazy_[node];

        lazy_[node * 2] += value;
        sum_[node * 2] += value * (middle - left + 1);

        lazy_[node * 2 + 1] += value;
        sum_[node * 2 + 1] += value * (right - middle);

        lazy_[node] = 0;  // 下传完就清空
    }

    void rangeAdd(int node, int left, int right, int queryLeft, int queryRight,
                  long long value) {
        if (queryRight < left || right < queryLeft) return;  // 完全不相交

        if (queryLeft <= left && right <= queryRight) {  // 完全覆盖
            sum_[node] += value * (right - left + 1);
            lazy_[node] += value;
            return;
        }

        pushDown(node, left, right);  // 部分覆盖，先把标记传下去
        int middle = left + (right - left) / 2;
        rangeAdd(node * 2, left, middle, queryLeft, queryRight, value);
        rangeAdd(node * 2 + 1, middle + 1, right, queryLeft, queryRight, value);
        sum_[node] = sum_[node * 2] + sum_[node * 2 + 1];
    }

    long long rangeSum(int node, int left, int right, int queryLeft,
                       int queryRight) {
        if (queryRight < left || right < queryLeft) return 0;
        if (queryLeft <= left && right <= queryRight) return sum_[node];

        pushDown(node, left, right);
        int middle = left + (right - left) / 2;
        return rangeSum(node * 2, left, middle, queryLeft, queryRight) +
               rangeSum(node * 2 + 1, middle + 1, right, queryLeft, queryRight);
    }

    int size_;
    vector<long long> sum_;
    vector<long long> lazy_;
};

// ------------------------------------------------------------
// 三、离散化
//
// 用途：数值范围很大（比如 1e9）但个数不多（比如 1e5），
//   先把它们排序去重，再把每个值换成它在排序数组里的下标（从 1 开始）。
//   这样原来的数值关系不变，却可以塞进树状数组 / 线段树。
//
// 时间复杂度 O(n log n)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<int> discretize(const vector<long long>& numbers) {
    vector<long long> sorted = numbers;
    std::sort(sorted.begin(), sorted.end());
    sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());

    vector<int> ranks(numbers.size(), 0);
    for (int i = 0; i < (int)numbers.size(); ++i) {
        // lower_bound 找第一个 >= 的位置，再 +1 变成从 1 开始的下标
        ranks[i] = (int)(std::lower_bound(sorted.begin(), sorted.end(),
                                          numbers[i]) -
                         sorted.begin()) + 1;
    }
    return ranks;
}

int main() {
    // 示例 1：树状数组
    FenwickTree fenwick(6);
    vector<long long> numbers = {1, 3, 5, 7, 9, 11};  // 下标 1..6
    for (int i = 1; i <= 6; ++i) {
        fenwick.add(i, numbers[i - 1]);
    }
    cout << "树状数组：[1, 3, 5, 7, 9, 11]" << endl;
    cout << "  前 3 个数的和 = " << fenwick.prefixSum(3)
         << "，区间 [2, 5] 的和 = " << fenwick.rangeSum(2, 5) << endl;
    assert(fenwick.prefixSum(3) == 9);        // 1 + 3 + 5
    assert(fenwick.rangeSum(2, 5) == 24);     // 3 + 5 + 7 + 9

    fenwick.add(4, 3);                        // 第 4 个数 7 -> 10
    assert(fenwick.rangeSum(4, 4) == 10);
    assert(fenwick.rangeSum(1, 6) == 39);     // 1+3+5+10+9+11
    cout << "  第 4 个数加 3 后，总和 = " << fenwick.rangeSum(1, 6) << endl;

    // 示例 2：线段树区间加 + 区间和
    SegmentTree segmentTree({1, 2, 3, 4, 5});
    cout << "线段树：[1, 2, 3, 4, 5]" << endl;
    assert(segmentTree.rangeSum(0, 4) == 15);
    segmentTree.rangeAdd(1, 3, 10);           // [1, 3] 每个加 10
    assert(segmentTree.rangeSum(0, 4) == 45);  // 1 + 12 + 13 + 14 + 5
    assert(segmentTree.rangeSum(1, 3) == 39);
    cout << "  [1, 3] 每个加 10 后，区间 [1, 3] 的和 = "
         << segmentTree.rangeSum(1, 3) << "，总和 = "
         << segmentTree.rangeSum(0, 4) << endl;

    segmentTree.rangeAdd(0, 4, 1);            // 全部加 1
    assert(segmentTree.rangeSum(0, 4) == 50);
    assert(segmentTree.rangeSum(2, 2) == 14);
    cout << "  全部加 1 后，总和 = " << segmentTree.rangeSum(0, 4) << endl;

    // 示例 3：离散化
    vector<long long> bigNumbers = {1000000000, 5, 999, 5, 42};
    vector<int> ranks = discretize(bigNumbers);
    cout << "离散化：[1000000000, 5, 999, 5, 42] -> ";
    for (int rank : ranks) cout << rank << " ";
    cout << endl;
    assert((ranks == vector<int>{4, 1, 3, 1, 2}));

    // 离散化之后可以直接当树状数组的下标用
    FenwickTree rankTree(4);
    for (int i = 0; i < (int)ranks.size(); ++i) {
        rankTree.add(ranks[i], bigNumbers[i]);
    }
    assert(rankTree.prefixSum(2) == 52);   // 秩 <= 2 的是 5 + 5 + 42
    cout << "  用离散化后的秩建树状数组，前 2 个秩的元素和 = "
         << rankTree.prefixSum(2) << endl;

    // 边界：空数组 / 单元素
    FenwickTree emptyTree(1);
    assert(emptyTree.prefixSum(0) == 0);
    SegmentTree singleTree({7});
    assert(singleTree.rangeSum(0, 0) == 7);
    singleTree.rangeAdd(0, 0, 3);
    assert(singleTree.rangeSum(0, 0) == 10);
    assert(discretize({}).empty());

    cout << "18 树状数组 / 线段树 / 离散化：全部示例通过" << endl;
    return 0;
}
