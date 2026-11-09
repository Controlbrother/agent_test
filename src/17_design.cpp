// ============================================================
// 专题 17：设计题（LRU 缓存 / Trie 前缀树 / O(1) 随机集合）
//
// 适用题型：LRU 缓存、LFU 缓存、实现 Trie、常数时间插入删除随机取、
//           最小栈（见 06_stack_queue.cpp）。
//
// 核心思想：
//   设计题考的是"把两种数据结构组合起来"，套路非常固定：
//     哈希表负责 O(1) 定位，链表 / 堆 / 数组负责维护顺序。
//
//   - LRU：哈希表（key -> 节点） + 双向链表（最近使用的在头部）；
//   - Trie：多叉树，每个节点有 26 个子节点指针 + "是否是单词结尾"标记；
//   - 随机集合：数组存值 + 哈希表存"值 -> 数组下标"，删除时把最后一个
//     元素挪到被删位置，保证删除也是 O(1)。
//
// 时间复杂度：每个操作 O(1)（Trie 是 O(单词长度)）
// 空间复杂度：O(容量) / O(总字符数)
//
// 易错点：
//   1. LRU 的哨兵（dummy head/tail）能消掉大量空指针判断，不要省。
//   2. LRU 的 put 更新已有 key 时，别忘了把节点移到最前面。
//   3. 淘汰时同时要从哈希表里 erase 并 delete 节点，只做一半会内存泄漏
//      或者留下悬空指针。
//   4. 随机集合删除时，如果删的正好是最后一个元素，不要再交换一次。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/17_design.cpp -o /tmp/demo17
//   /tmp/demo17
// ============================================================

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

// ------------------------------------------------------------
// 一、LRU 缓存（LeetCode 146）
//
// 要求：get 和 put 都是 O(1)；容量满时淘汰"最久未使用"的那一个。
//
// 结构：
//   - 双向链表：头部是最近使用的，尾部是最久未使用的；
//   - 哈希表：key -> 链表节点指针，用来 O(1) 找到节点。
//
// 关键操作：
//   - 访问一个节点：从链表里摘下来，插到头部（表示"刚用过"）；
//   - 插入新节点：插到头部，如果超容量就删掉尾部那个。
// ------------------------------------------------------------
class LRUCache {
public:
    explicit LRUCache(int capacity) : capacity_(capacity) {
        // 两个哨兵节点：head <-> ... <-> tail
        head_ = new Node(0, 0);
        tail_ = new Node(0, 0);
        head_->next = tail_;
        tail_->prev = head_;
    }

    ~LRUCache() {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    // 禁止拷贝，避免两个对象管理同一批节点（工程细节，面试可以不写）
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    int get(int key) {
        auto found = position_.find(key);
        if (found == position_.end()) {
            return -1;  // 没有这个 key
        }
        Node* node = found->second;
        moveToFront(node);  // 访问过就算"最近使用"
        return node->value;
    }

    void put(int key, int value) {
        auto found = position_.find(key);
        if (found != position_.end()) {
            // key 已存在：更新值并移到最前面
            Node* node = found->second;
            node->value = value;
            moveToFront(node);
            return;
        }

        if ((int)position_.size() == capacity_) {
            Node* oldest = tail_->prev;  // 最久未使用的节点
            removeNode(oldest);
            position_.erase(oldest->key);
            delete oldest;
        }

        Node* node = new Node(key, value);
        insertAtFront(node);
        position_[key] = node;
    }

private:
    struct Node {
        int key;
        int value;
        Node* prev;
        Node* next;

        Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    // 把节点从链表里摘下来（前后接上）
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 把节点插到 head 后面（= 最近使用）
    void insertAtFront(Node* node) {
        node->next = head_->next;
        node->prev = head_;
        head_->next->prev = node;
        head_->next = node;
    }

    void moveToFront(Node* node) {
        removeNode(node);
        insertAtFront(node);
    }

    int capacity_;
    unordered_map<int, Node*> position_;  // key -> 节点
    Node* head_;                          // 哨兵：最近使用的一侧
    Node* tail_;                          // 哨兵：最久未使用的一侧
};

// ------------------------------------------------------------
// 二、Trie 前缀树（LeetCode 208）
//
// 用途：词频统计、前缀匹配、输入法联想、敏感词过滤。
//
// 结构：
//   每个节点有 26 个孩子指针（对应 a~z）和一个"单词结尾"标记。
//   insert：沿着字符一路往下走，没有节点就创建；
//   search：沿着字符走，走不动就 false，走完还要看是不是单词结尾；
//   startsWith：只要走得完就 true（不要求是完整单词）。
//
// 时间复杂度：每个操作 O(单词长度)，空间复杂度 O(总字符数 * 26)。
// ------------------------------------------------------------
class Trie {
public:
    Trie() : root_(new TrieNode()) {}

    ~Trie() { destroy(root_); }

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(const string& word) {
        TrieNode* current = root_;
        for (char c : word) {
            int index = c - 'a';
            if (current->children[index] == nullptr) {
                current->children[index] = new TrieNode();
            }
            current = current->children[index];
        }
        current->isWordEnd = true;  // 最后一个字符标记成单词结尾
    }

    bool search(const string& word) const {
        const TrieNode* node = findNode(word);
        return node != nullptr && node->isWordEnd;
    }

    bool startsWith(const string& prefix) const {
        return findNode(prefix) != nullptr;
    }

private:
    struct TrieNode {
        TrieNode* children[26];  // 26 个小写字母
        bool isWordEnd;

        TrieNode() : isWordEnd(false) {
            for (int i = 0; i < 26; ++i) children[i] = nullptr;
        }
    };

    // 沿着字符串走，走不通返回 nullptr
    const TrieNode* findNode(const string& text) const {
        const TrieNode* current = root_;
        for (char c : text) {
            int index = c - 'a';
            if (index < 0 || index >= 26) return nullptr;
            if (current->children[index] == nullptr) return nullptr;
            current = current->children[index];
        }
        return current;
    }

    // 递归释放整棵树（后序）
    void destroy(TrieNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < 26; ++i) destroy(node->children[i]);
        delete node;
    }

    TrieNode* root_;
};

// ------------------------------------------------------------
// 三、O(1) 时间插入、删除、随机取（LeetCode 380）
//
// 结构：
//   - 数组 values_：存所有元素，下标连续；
//   - 哈希表 indexOf_：值 -> 它在数组里的下标。
//
// insert：不存在就追加到数组末尾；
// remove：把"数组最后一个元素"挪到被删位置，再删掉末尾
//         （这样数组中间不会出现空洞，随机取才能保持等概率）；
// getRandom：在 [0, size) 里随机取一个下标。
//
// 时间复杂度 O(1)，空间复杂度 O(n)。
// ------------------------------------------------------------
class RandomizedSet {
public:
    bool insert(int value) {
        if (indexOf_.find(value) != indexOf_.end()) return false;
        indexOf_[value] = (int)values_.size();
        values_.push_back(value);
        return true;
    }

    bool remove(int value) {
        auto found = indexOf_.find(value);
        if (found == indexOf_.end()) return false;

        int deletedIndex = found->second;
        int lastValue = values_.back();

        // 把最后一个元素挪到被删位置
        values_[deletedIndex] = lastValue;
        indexOf_[lastValue] = deletedIndex;

        values_.pop_back();
        indexOf_.erase(value);
        return true;
    }

    int getRandom() const {
        int randomIndex = std::rand() % (int)values_.size();
        return values_[randomIndex];
    }

private:
    vector<int> values_;
    unordered_map<int, int> indexOf_;  // 值 -> 数组下标
};

int main() {
    // 示例 1：LRU 缓存
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    assert(cache.get(1) == 1);    // 1 变成最近使用
    cache.put(3, 3);              // 容量满了，淘汰最久未使用的 2
    assert(cache.get(2) == -1);
    cache.put(4, 4);              // 淘汰 1
    assert(cache.get(1) == -1);
    assert(cache.get(3) == 3);
    assert(cache.get(4) == 4);
    cout << "LRU 缓存：容量 2，依次 put(1,1) put(2,2) get(1) put(3,3) "
            "-> get(1)=1, get(2)=-1, get(3)=3" << endl;

    // 更新已有 key 的情形
    LRUCache cache2(1);
    cache2.put(1, 10);
    cache2.put(1, 20);
    assert(cache2.get(1) == 20);
    cout << "LRU 缓存：同一个 key 重复 put 会更新值，get(1) = 20" << endl;

    // 示例 2：Trie
    Trie trie;
    trie.insert("apple");
    assert(trie.search("apple") == true);
    assert(trie.search("app") == false);    // 只是前缀，不是完整单词
    assert(trie.startsWith("app") == true);
    trie.insert("app");
    assert(trie.search("app") == true);
    assert(trie.search("banana") == false);
    cout << "Trie：插入 apple 后 search(app)=false、startsWith(app)=true；"
            "再插入 app 后 search(app)=true" << endl;

    // 示例 3：O(1) 随机集合
    std::srand(12345);  // 固定随机种子，保证每次运行结果一致
    RandomizedSet randomSet;
    assert(randomSet.insert(1) == true);
    assert(randomSet.insert(2) == true);
    assert(randomSet.remove(1) == true);
    assert(randomSet.insert(2) == false);   // 2 已经在集合里
    assert(randomSet.remove(3) == false);   // 3 不在集合里
    int randomValue = randomSet.getRandom();
    assert(randomValue == 2);               // 集合里只剩 2
    cout << "RandomizedSet：插入 1、2，删 1 后随机取值 = " << randomValue << endl;

    // 删除中间元素后，数组仍然连续、随机取仍然是集合里的值
    RandomizedSet biggerSet;
    for (int value = 0; value < 10; ++value) biggerSet.insert(value);
    assert(biggerSet.remove(4) == true);
    for (int i = 0; i < 20; ++i) {
        int value = biggerSet.getRandom();
        assert(value >= 0 && value < 10 && value != 4);
    }
    cout << "RandomizedSet：0..9 删除 4 后连续随机取值都合法" << endl;

    cout << "17 设计题：全部示例通过" << endl;
    return 0;
}
