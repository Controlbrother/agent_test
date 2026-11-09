// ============================================================
// 专题 09：图（BFS / DFS / 拓扑排序 / 并查集 / Dijkstra）
//
// 适用题型：岛屿数量、课程表（判环）、省份数量、冗余连接、
//           网络延迟时间（单源最短路）、迷宫最短路。
//
// 核心思想：
//   图题先分清三件事：有向还是无向、有没有边权、要不要判环。
//     - 无权最短路（边数最少）  -> BFS
//     - 非负权最短路            -> Dijkstra（优先队列）
//     - 连通性 / 合并集合       -> 并查集
//     - 依赖顺序 / 判环         -> 拓扑排序（BFS 入度法）
//
// 图的表示：本文件统一用"邻接表" vector<vector<int>>，
//   graph[u] 存 u 能一步走到的所有点。带权图用 vector<vector<pair<int,int>>>，
//   pair 是 (邻点, 边权)。
//
// 时间复杂度：
//   BFS / DFS O(V + E)，拓扑排序 O(V + E)，并查集近似 O(1) 每次，
//   Dijkstra O((V + E) log V)
// 空间复杂度：O(V + E)
//
// 易错点：
//   1. BFS 要在"入队时"标记已访问，出队时才标记会重复入队。
//   2. Dijkstra 不能处理负权边，有负权要用 Bellman-Ford。
//   3. 拓扑排序最后要检查"排出来的点数是否等于 V"，不等说明有环。
//   4. 并查集的 find 要带路径压缩，否则可能退化成链表。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/09_graph.cpp -o /tmp/demo09
//   /tmp/demo09
// ============================================================

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::pair;
using std::priority_queue;
using std::queue;
using std::vector;

// ------------------------------------------------------------
// 1. 无权图 BFS 最短路（LeetCode 743 的简化版）
//
// 思路：
//   从起点开始一层一层向外扩展，第一次到达某个点时，
//   走的边数一定最少。dist[v] 记录起点到 v 的最少边数，-1 表示没到过。
//
// 时间复杂度 O(V + E)，空间复杂度 O(V)。
// ------------------------------------------------------------
vector<int> bfsShortestPath(const vector<vector<int>>& graph, int start) {
    int n = (int)graph.size();
    vector<int> distance(n, -1);
    queue<int> pending;
    distance[start] = 0;
    pending.push(start);

    while (!pending.empty()) {
        int current = pending.front();
        pending.pop();

        for (int next : graph[current]) {
            if (distance[next] != -1) continue;  // 已经访问过
            distance[next] = distance[current] + 1;
            pending.push(next);                  // 入队时就标记，避免重复入队
        }
    }
    return distance;
}

// ------------------------------------------------------------
// 2. 网格 DFS：岛屿数量（LeetCode 200）
//
// 思路：
//   遍历每个格子，遇到 '1' 就把答案 +1，然后用 DFS 把
//   与它相连的所有 '1' 都"淹掉"（改成 '0'），避免重复计数。
//
// 时间复杂度 O(rows * cols)，空间复杂度 O(rows * cols)（递归栈最坏情况）。
//
// 易错点：网格 DFS 递归深度可能很大，几万格时改用 BFS 或显式栈。
// ------------------------------------------------------------
void floodIsland(vector<vector<char>>& grid, int row, int col) {
    if (row < 0 || row >= (int)grid.size()) return;
    if (col < 0 || col >= (int)grid[0].size()) return;
    if (grid[row][col] != '1') return;

    grid[row][col] = '0';  // 标记已访问（淹掉）

    floodIsland(grid, row + 1, col);
    floodIsland(grid, row - 1, col);
    floodIsland(grid, row, col + 1);
    floodIsland(grid, row, col - 1);
}

int countIslands(vector<vector<char>> grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    int count = 0;
    for (int row = 0; row < (int)grid.size(); ++row) {
        for (int col = 0; col < (int)grid[0].size(); ++col) {
            if (grid[row][col] == '1') {
                ++count;
                floodIsland(grid, row, col);
            }
        }
    }
    return count;
}

// ------------------------------------------------------------
// 3. 拓扑排序（LeetCode 207 / 210）
//
// 思路（BFS + 入度）：
//   1. 统计每个点的入度（有多少条边指向它）；
//   2. 入度为 0 的点先入队（它们不依赖任何人）；
//   3. 每次取出一个点放进答案，把它指向的点的入度减 1，
//      减到 0 就入队；
//   4. 最后答案长度等于点数说明无环，否则有环。
//
// 时间复杂度 O(V + E)，空间复杂度 O(V)。
// ------------------------------------------------------------
vector<int> topologicalSort(int n, const vector<vector<int>>& graph) {
    vector<int> indegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            ++indegree[v];
        }
    }

    queue<int> ready;  // 入度为 0 的点，随时可以处理
    for (int i = 0; i < n; ++i) {
        if (indegree[i] == 0) ready.push(i);
    }

    vector<int> order;
    while (!ready.empty()) {
        int current = ready.front();
        ready.pop();
        order.push_back(current);

        for (int next : graph[current]) {
            --indegree[next];
            if (indegree[next] == 0) ready.push(next);
        }
    }

    if ((int)order.size() != n) {
        return {};  // 有环，无法排完
    }
    return order;
}

// ------------------------------------------------------------
// 4. 并查集（Union-Find / DSU）
//
// 思路：
//   用 parent[x] 记录 x 的父节点，同一个集合的所有节点最终指向同一个根。
//   find(x)：一路往上找到根，顺便把路径上的节点直接挂到根上（路径压缩）。
//   unite(a, b)：把两个根合并，按大小合并（小树挂到大树下面）保持树矮。
//
// 时间复杂度：单次操作近似 O(1)（严格说是反阿克曼函数）
// 空间复杂度 O(n)
//
// 易错点：
//   - find 写成递归没问题，但路径压缩不能漏；
//   - 合并时比较的是"根"，不是节点本身。
// ------------------------------------------------------------
class DisjointSet {
public:
    explicit DisjointSet(int n) : parent(n), size(n, 1) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // 路径压缩
        }
        return parent[x];
    }

    bool unite(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA == rootB) return false;  // 本来就在同一个集合

        if (size[rootA] < size[rootB]) std::swap(rootA, rootB);
        parent[rootB] = rootA;   // 小树挂到大树下面
        size[rootA] += size[rootB];
        return true;
    }

    bool same(int a, int b) { return find(a) == find(b); }

    int componentCount() const {
        int count = 0;
        for (int i = 0; i < (int)parent.size(); ++i) {
            if (parent[i] == i) ++count;
        }
        return count;
    }

private:
    vector<int> parent;
    vector<int> size;  // 每个根所在集合的大小
};

// ------------------------------------------------------------
// 5. Dijkstra（非负权单源最短路）
//
// 思路：
//   贪心 + 优先队列。每次从"还没确定的点"里取出距离最小的点 u，
//   它的最短距离就确定了（因为没有负权，绕路只会更远）；
//   再用 u 去更新它邻居的距离。
//
// 优先队列里存 (距离, 点)，默认是大顶堆，所以用 greater<> 变成小顶堆。
// 同一个点可能被多次入队，出队时距离更大的那些是过期数据，直接跳过。
//
// 时间复杂度 O((V + E) log V)，空间复杂度 O(V + E)。
//
// 易错点：
//   - 距离用 long long，防止多段边权相加溢出 int；
//   - INF 取 LLONG_MAX / 4，避免加法溢出。
// ------------------------------------------------------------
vector<long long> dijkstra(int n,
                           const vector<vector<pair<int, int>>>& graph,
                           int start) {
    const long long INF = std::numeric_limits<long long>::max() / 4;
    vector<long long> distance(n, INF);
    distance[start] = 0;

    using State = pair<long long, int>;  // (当前距离, 点)
    priority_queue<State, vector<State>, std::greater<State>> pending;
    pending.push({0, start});

    while (!pending.empty()) {
        auto [currentDistance, current] = pending.top();
        pending.pop();

        if (currentDistance > distance[current]) continue;  // 过期数据

        for (auto [next, weight] : graph[current]) {
            long long candidate = currentDistance + weight;
            if (candidate < distance[next]) {
                distance[next] = candidate;
                pending.push({candidate, next});
            }
        }
    }
    return distance;
}

int main() {
    // 示例 1：无权图 BFS 最短路
    // 0-1-2-3 是一条链，0 还直连 3
    vector<vector<int>> graph = {
        {1, 3},  // 0 的邻居
        {0, 2},  // 1 的邻居
        {1, 3},  // 2 的邻居
        {0, 2},  // 3 的邻居
    };
    vector<int> distances = bfsShortestPath(graph, 0);
    cout << "BFS 最短路（从 0 出发）：";
    for (int d : distances) cout << d << " ";
    cout << endl;
    assert((distances == vector<int>{0, 1, 2, 1}));

    // 示例 2：岛屿数量
    vector<vector<char>> grid = {
        {'1', '1', '0', '0', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '1', '0', '0'},
        {'0', '0', '0', '1', '1'},
    };
    int islands = countIslands(grid);
    cout << "岛屿数量 = " << islands << endl;
    assert(islands == 3);

    // 示例 3：拓扑排序
    // 课程依赖：0 <- 1，0 <- 2，1 <- 3
    vector<vector<int>> courseGraph = {{1, 2}, {3}, {3}, {}};
    vector<int> order = topologicalSort(4, courseGraph);
    cout << "拓扑排序：";
    for (int course : order) cout << course << " ";
    cout << endl;
    assert((int)order.size() == 4);
    assert(order.back() == 3);  // 3 最后才能学

    // 有环的情况：0 -> 1 -> 0
    vector<vector<int>> cycleGraph = {{1}, {0}};
    assert(topologicalSort(2, cycleGraph).empty());

    // 示例 4：并查集
    DisjointSet dsu(5);
    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(3, 4);
    cout << "并查集：0-1-2 连通，3-4 连通，连通分量数 = "
         << dsu.componentCount() << endl;
    assert(dsu.same(0, 2) == true);
    assert(dsu.same(0, 3) == false);
    assert(dsu.componentCount() == 2);

    // 示例 5：Dijkstra
    // 0 --1-- 1 --2-- 2，0 --4-- 2
    vector<vector<pair<int, int>>> weightedGraph = {
        {{1, 1}, {2, 4}},
        {{2, 2}},
        {},
    };
    vector<long long> shortest = dijkstra(3, weightedGraph, 0);
    cout << "Dijkstra 最短路（从 0 出发）：";
    for (long long d : shortest) cout << d << " ";
    cout << endl;
    assert((shortest == vector<long long>{0, 1, 3}));  // 0->1->2 比 0->2 短
    assert(dijkstra(3, weightedGraph, 1)[0] ==
           std::numeric_limits<long long>::max() / 4);  // 1 到不了 0

    cout << "09 图：全部示例通过" << endl;
    return 0;
}
