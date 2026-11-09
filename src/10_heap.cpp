// ============================================================
// 专题 10：堆 / 优先队列
//
// 本节实现：第 K 大元素、前 K 个高频元素、数据流中位数（双堆）、
//           合并 K 个有序链表。
//
// 同思路可解：会议室 II（按开始时间扫，用小顶堆存结束时间）、
//           数据流第 K 大（同样是小顶堆，容量固定为 K）。
//
// 核心思想：
//   堆只关心"当前最大（或最小）的那个元素"。
//   题目里出现"第 K 大/小""前 K 个""动态取最值"，第一反应就是堆。
//
//   C++ 的 priority_queue 默认是"大顶堆"（堆顶最大）：
//     priority_queue<int> maxHeap;
//   要小顶堆就写全三个模板参数：
//     priority_queue<int, vector<int>, greater<int>> minHeap;
//
// 时间复杂度：插入 / 弹出 O(log n)，取堆顶 O(1)
// 空间复杂度：O(k) 或 O(n)
//
// 易错点：
//   1. 求"前 K 大"用小顶堆（堆里保留最大的 K 个，堆顶是其中最小的一个，
//      来了更大的就替换掉堆顶）。求"前 K 小"反过来用大顶堆。
//   2. priority_queue 不支持删除任意元素，要删除就"惰性删除"：
//      出队时判断是否过期。
//   3. 自定义类型要写比较器，注意 greater 对应的是小顶堆，容易写反。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/10_heap.cpp -o /tmp/demo10
//   /tmp/demo10
// ============================================================

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::greater;
using std::pair;
using std::priority_queue;
using std::unordered_map;
using std::vector;

// ------------------------------------------------------------
// 题目 1：数组中的第 K 个最大元素（LeetCode 215）
//
// 思路：
//   维护一个大小为 k 的小顶堆，堆里始终是"目前最大的 k 个数"。
//   遍历完数组后，堆顶就是这 k 个数里最小的，也就是第 k 大。
//
// 时间复杂度 O(n log k)，空间复杂度 O(k)。
// 另一种 O(n) 的快速选择算法见 15_sorting.cpp。
// ------------------------------------------------------------
int findKthLargest(const vector<int>& numbers, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;  // 小顶堆

    for (int value : numbers) {
        minHeap.push(value);
        if ((int)minHeap.size() > k) {
            minHeap.pop();  // 弹掉当前 k 个里最小的，保证堆里是最大的 k 个
        }
    }
    return minHeap.top();
}

// ------------------------------------------------------------
// 题目 2：前 K 个高频元素（LeetCode 347）
//
// 思路：
//   1. 先用哈希表统计每个数字出现的次数；
//   2. 再用小顶堆维护"频率最高的 k 个"，堆顶是这 k 个里频率最低的。
//
// 时间复杂度 O(n log k)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<int> topKFrequent(const vector<int>& numbers, int k) {
    unordered_map<int, int> frequency;
    for (int value : numbers) ++frequency[value];

    // 堆里存 (频率, 数字)，按频率比较
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>
        minHeap;

    for (auto& entry : frequency) {
        int value = entry.first;
        int count = entry.second;
        minHeap.push({count, value});
        if ((int)minHeap.size() > k) {
            minHeap.pop();
        }
    }

    vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().second);
        minHeap.pop();
    }
    // 上面得到的是从低频到高频，翻转成常见输出顺序
    std::reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------------
// 题目 3：数据流的中位数（LeetCode 295）—— 双堆
//
// 思路：
//   维护两个堆：
//     - 大顶堆 small：存较小的一半，堆顶是这一半里最大的；
//     - 小顶堆 large：存较大的一半，堆顶是这一半里最小的。
//   保证 small 的元素个数 >= large 的元素个数，且相差不超过 1。
//   中位数：奇数个就是 small 的堆顶；偶数个是两个堆顶的平均。
//
// 每次插入分三步：先塞进 small，再把 small 的最大值挪到 large，
//   如果 large 比 small 多就挪回来 —— 这样两个堆的大小关系永远正确。
//
// 时间复杂度：插入 O(log n)，查询中位数 O(1)；空间复杂度 O(n)。
// ------------------------------------------------------------
class MedianFinder {
public:
    void addNumber(int value) {
        small.push(value);              // 1. 先放进较小的一半
        large.push(small.top());        // 2. 把较小一半里最大的挪到较大一半
        small.pop();

        if (large.size() > small.size()) {  // 3. 保持 small 不少于 large
            small.push(large.top());
            large.pop();
        }
    }

    double findMedian() const {
        if (small.size() > large.size()) {
            return small.top();  // 奇数个，中位数就是中间那个
        }
        return (small.top() + large.top()) / 2.0;
    }

private:
    priority_queue<int> small;                             // 大顶堆
    priority_queue<int, vector<int>, greater<int>> large;  // 小顶堆
};

// ------------------------------------------------------------
// 题目 4：合并 K 个升序链表（LeetCode 23）
//
// 思路：
//   把每个链表的头节点放进小顶堆（按节点值比较），
//   每次弹出最小的节点接到结果链表后面，再把它的下一个节点入堆。
//   堆里最多有 K 个节点，所以每次操作 O(log K)。
//
// 时间复杂度 O(N log K)，N 是节点总数；空间复杂度 O(K)。
// ------------------------------------------------------------
struct ListNode {
    int value;
    ListNode* next;
    explicit ListNode(int v) : value(v), next(nullptr) {}
};

// 自定义比较器：值小的排在堆顶（小顶堆）
struct CompareNode {
    bool operator()(ListNode* a, ListNode* b) const {
        return a->value > b->value;  // 注意是大于号：> 才能得到小顶堆
    }
};

ListNode* mergeKLists(const vector<ListNode*>& lists) {
    priority_queue<ListNode*, vector<ListNode*>, CompareNode> minHeap;

    for (ListNode* head : lists) {
        if (head != nullptr) minHeap.push(head);
    }

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (!minHeap.empty()) {
        ListNode* smallest = minHeap.top();
        minHeap.pop();

        tail->next = smallest;
        tail = tail->next;

        if (smallest->next != nullptr) {
            minHeap.push(smallest->next);
        }
    }
    return dummy.next;
}

// 工具函数：按数组建链表
ListNode* createList(const vector<int>& numbers) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (int value : numbers) {
        tail->next = new ListNode(value);
        tail = tail->next;
    }
    return dummy.next;
}

// 工具函数：打印链表
void printList(ListNode* head) {
    for (ListNode* current = head; current != nullptr; current = current->next) {
        cout << current->value;
        if (current->next != nullptr) cout << " -> ";
    }
    cout << endl;
}

// 工具函数：释放链表
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

int main() {
    // 示例 1：第 K 大
    vector<int> numbers = {3, 2, 1, 5, 6, 4};
    cout << "第 2 大元素：数组 [3, 2, 1, 5, 6, 4] -> "
         << findKthLargest(numbers, 2) << endl;
    assert(findKthLargest(numbers, 2) == 5);
    assert(findKthLargest({1}, 1) == 1);

    // 示例 2：前 K 个高频元素
    vector<int> frequent = topKFrequent({1, 1, 1, 2, 2, 3}, 2);
    cout << "前 2 个高频元素：";
    for (int value : frequent) cout << value << " ";
    cout << endl;
    assert(frequent.size() == 2);
    assert(frequent[0] == 1 && frequent[1] == 2);

    // 示例 3：数据流中位数
    MedianFinder finder;
    finder.addNumber(1);
    finder.addNumber(2);
    double median1 = finder.findMedian();
    finder.addNumber(3);
    double median2 = finder.findMedian();
    cout << "数据流中位数：[1, 2] -> " << median1 << "，[1, 2, 3] -> "
         << median2 << endl;
    assert(median1 == 1.5);
    assert(median2 == 2.0);

    // 示例 4：合并 K 个有序链表
    vector<ListNode*> lists = {
        createList({1, 4, 5}),
        createList({1, 3, 4}),
        createList({2, 6}),
    };
    ListNode* merged = mergeKLists(lists);
    cout << "合并 3 个有序链表：";
    printList(merged);
    assert(merged->value == 1);
    freeList(merged);

    cout << "10 堆：全部示例通过" << endl;
    return 0;
}
