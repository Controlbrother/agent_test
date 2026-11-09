// ============================================================
// 专题 05：链表
//
// 本节实现：反转链表、合并两个有序链表、删除倒数第 N 个节点、
//           环形链表找入环点、找中点。
//
// 同思路可解：重排链表（找中点 + 反转 + 合并，正好是本节的三个函数拼起来）、
//           反转链表 II（局部反转）、合并 K 个有序链表（用堆，见 10_heap.cpp）。
//
// 核心思想：
//   链表题的套路只有三个，几乎所有题都是它们的组合：
//     1. 哑节点（dummy）：在 head 前面加一个假节点，
//        这样"删除头节点""在头部插入"就不用特判了；
//     2. 快慢指针：慢指针走一步、快指针走两步，
//        用来找中点、判断有没有环；
//     3. 就地反转：用 prev / current / next 三个指针把箭头反过来。
//
// 时间复杂度：每个操作都是 O(n) 或 O(n) 遍历一次
// 空间复杂度：O(1)（只用了几个指针，不额外开数组）
//
// 易错点：
//   1. 反转时先保存 next，再改 current->next，顺序反了就断链。
//   2. 快慢指针找中点的循环条件 while (fast && fast->next)，
//      这决定了偶数长度时停在中点还是中点前一个。
//   3. 删除节点后别忘了 delete（本文件用 new 创建节点）。
//   4. 空链表、只有一个节点要单独想一遍，别直接解引用空指针。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/05_linked_list.cpp -o /tmp/demo05
//   /tmp/demo05
// ============================================================

#include <cassert>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// 单链表节点定义（LeetCode 上的写法）
struct ListNode {
    int value;
    ListNode* next;

    explicit ListNode(int v) : value(v), next(nullptr) {}
};

// ------------------------------------------------------------
// 工具函数：用数组创建链表，返回头节点
//
// 时间复杂度 O(n)，空间复杂度 O(n)（n 个节点）。
// ------------------------------------------------------------
ListNode* createList(const vector<int>& numbers) {
    ListNode dummy(0);       // 哑节点，省掉"第一个节点"的特判
    ListNode* tail = &dummy;
    for (int value : numbers) {
        tail->next = new ListNode(value);
        tail = tail->next;
    }
    return dummy.next;
}

// 工具函数：打印链表，例如 1 -> 2 -> 3 -> null
void printList(ListNode* head) {
    ListNode* current = head;
    bool first = true;
    while (current != nullptr) {
        if (!first) cout << " -> ";
        cout << current->value;
        first = false;
        current = current->next;
    }
    cout << " -> null" << endl;
}

// 工具函数：释放整条链表，避免内存泄漏
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

// ------------------------------------------------------------
// 题目 1：反转链表（LeetCode 206）
//
// 思路：
//   从头节点开始，依次把每个节点的 next 指向它的前一个节点。
//   prev 表示"已经反转好的部分"的头，current 表示"还没处理的部分"的头。
//   每轮先用 next 记住后面的节点，再反转 current->next，最后两个指针前移。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;   // 反转后它会是新的头
    ListNode* current = head;

    while (current != nullptr) {
        ListNode* next = current->next;  // 1. 先存后继，否则改完就找不到了
        current->next = prev;            // 2. 反转箭头
        prev = current;                  // 3. prev 前移
        current = next;                  // 4. current 前移
    }
    return prev;
}

// ------------------------------------------------------------
// 题目 2：合并两个有序链表（LeetCode 21）
//
// 思路：
//   用哑节点当结果链表的头，用 tail 指向结果链表最后一个节点。
//   每次比较两个链表的头，把小的那个接在 tail 后面。
//   最后把没走完的那条链表整段接上。
//
// 时间复杂度 O(n + m)，空间复杂度 O(1)（复用原节点）。
// ------------------------------------------------------------
ListNode* mergeTwoLists(ListNode* first, ListNode* second) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (first != nullptr && second != nullptr) {
        if (first->value <= second->value) {
            tail->next = first;
            first = first->next;
        } else {
            tail->next = second;
            second = second->next;
        }
        tail = tail->next;
    }

    // 剩下的那一段直接接上
    tail->next = (first != nullptr) ? first : second;
    return dummy.next;
}

// ------------------------------------------------------------
// 题目 3：删除链表的倒数第 N 个节点（LeetCode 19）
//
// 思路：
//   fast 先走 N + 1 步，然后 fast 和 slow 一起走。
//   当 fast 走到 nullptr 时，slow 正好停在"要删除节点的前一个"。
//   为什么是 N + 1 步？因为要让 slow 停在待删节点的前驱，
//   否则没有前驱指针就没法删除。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 用哑节点处理"删除头节点"的情况。
//
// 边界：题目保证 1 <= n <= 链表长度；这里加一个防御性检查，
//   传入非法 n 时原样返回，不会解引用空指针。
// ------------------------------------------------------------
ListNode* removeNthFromEnd(ListNode* head, int n) {
    if (n <= 0) return head;

    ListNode dummy(0);
    dummy.next = head;

    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    for (int i = 0; i <= n; ++i) {
        if (fast == nullptr) return head;  // n 超过链表长度，直接不删
        fast = fast->next;                 // fast 先走 n + 1 步
    }
    while (fast != nullptr) {
        fast = fast->next;
        slow = slow->next;
    }

    ListNode* toDelete = slow->next;
    slow->next = toDelete->next;  // 跨过待删节点
    delete toDelete;
    return dummy.next;
}

// ------------------------------------------------------------
// 题目 4：环形链表 II —— 找入环点（LeetCode 142）
//
// 思路（数学结论）：
//   快慢指针相遇后，让一个指针从头节点出发、另一个从相遇点出发，
//   两个指针每次各走一步，再次相遇的位置就是入环点。
//   推导：设头到环入口距离 a，入口到相遇点距离 b，环长 c，
//   相遇时快指针走了 a + b + k*c，慢指针走了 a + b，
//   因为快指针速度是慢指针两倍：2(a + b) = a + b + k*c，
//   所以 a = k*c - b，即"从头走 a 步"和"从相遇点走 a 步"会同时到入口。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 返回 nullptr 表示没有环。
// ------------------------------------------------------------
ListNode* detectCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {              // 相遇了，说明有环
            ListNode* fromHead = head;
            ListNode* fromMeeting = slow;
            while (fromHead != fromMeeting) {
                fromHead = fromHead->next;
                fromMeeting = fromMeeting->next;
            }
            return fromHead;             // 入环点
        }
    }
    return nullptr;                      // 走到空，没有环
}

// ------------------------------------------------------------
// 题目 5：找链表中点（LeetCode 876）
//
// 思路：
//   慢指针走一步、快指针走两步。快指针到末尾时，慢指针在中点。
//   偶数个节点时返回"靠后"的那个中点（因为循环条件是 fast && fast->next）。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

int main() {
    // 示例 1：反转链表
    ListNode* list1 = createList({1, 2, 3, 4, 5});
    cout << "反转前：";
    printList(list1);
    list1 = reverseList(list1);
    cout << "反转后：";
    printList(list1);
    assert(middleNode(list1)->value == 3);  // 5 个节点，中点是第 3 个
    freeList(list1);

    // 示例 2：合并两个有序链表
    ListNode* listA = createList({1, 2, 4});
    ListNode* listB = createList({1, 3, 4});
    ListNode* merged = mergeTwoLists(listA, listB);
    cout << "合并有序链表：";
    printList(merged);
    assert(merged->value == 1 && merged->next->value == 1);
    freeList(merged);

    // 示例 3：删除倒数第 N 个节点
    ListNode* list2 = createList({1, 2, 3, 4, 5});
    list2 = removeNthFromEnd(list2, 2);  // 删掉 4
    cout << "删除倒数第 2 个后：";
    printList(list2);
    assert(list2->next->next->next->value == 5);
    // 边界：n 非法时原样返回，不崩溃
    assert(removeNthFromEnd(list2, 0) == list2);
    assert(removeNthFromEnd(list2, 99) == list2);
    freeList(list2);

    // 示例 4：环形链表找入环点
    // 手工造一个环：1 -> 2 -> 3 -> 4 -> 2（入环点是 2）
    ListNode* node1 = new ListNode(1);
    ListNode* node2 = new ListNode(2);
    ListNode* node3 = new ListNode(3);
    ListNode* node4 = new ListNode(4);
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node2;  // 成环

    ListNode* cycleStart = detectCycle(node1);
    cout << "环形链表入环点的值 = "
         << (cycleStart != nullptr ? cycleStart->value : -1) << endl;
    assert(cycleStart == node2);

    // 有环的链表不能直接 freeList，先断开环再释放
    node4->next = nullptr;
    freeList(node1);

    // 示例 5：无环链表的检测
    ListNode* noCycle = createList({1, 2, 3});
    assert(detectCycle(noCycle) == nullptr);
    assert(middleNode(noCycle)->value == 2);  // 3 个节点，中点是第 2 个
    freeList(noCycle);

    // 边界：空链表
    assert(reverseList(nullptr) == nullptr);
    assert(detectCycle(nullptr) == nullptr);

    cout << "05 链表：全部示例通过" << endl;
    return 0;
}
