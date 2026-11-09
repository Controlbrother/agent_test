// ============================================================
// 专题 07：二叉树
//
// 适用题型：三种遍历（递归 + 迭代）、层序遍历、最大深度、
//           翻转二叉树、对称二叉树、验证二叉搜索树（BST）、
//           最近公共祖先（LCA）、从前序和中序构造二叉树。
//
// 核心思想：
//   二叉树题 = 遍历框架 + 在某个时机做事。
//   先想清楚"这件事应该在前序、中序还是后序做"，代码自然就出来了：
//     - 前序（根左右）：自顶向下传信息，例如传深度、传路径；
//     - 中序（左根右）：二叉搜索树的中序是升序；
//     - 后序（左右根）：自底向上收集信息，例如求高度、求 LCA。
//
// 时间复杂度：每个函数都是 O(n)，每个节点访问一次
// 空间复杂度：O(h)，h 是树高（递归栈）；层序遍历是 O(宽度)
//
// 易错点：
//   1. 递归函数第一行先判空：if (root == nullptr) return ...;
//   2. 层序遍历必须先记下 q.size()，边遍历边取 size 会串层。
//   3. 验证 BST 不能只比较父子节点，要传上下界或用中序。
//   4. 递归遍历要 delete 节点的话注意顺序（本文件用后序释放）。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/07_binary_tree.cpp -o /tmp/demo07
//   /tmp/demo07
// ============================================================

#include <cassert>
#include <climits>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::queue;
using std::stack;
using std::vector;

// 二叉树节点定义（LeetCode 上的写法）
struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;

    explicit TreeNode(int v) : value(v), left(nullptr), right(nullptr) {}
};

// ------------------------------------------------------------
// 工具函数：按层序数组建树，空位用 -1 的占位符表示（简化版，方便写示例）
//
// 说明：真正的 LeetCode 输入里 null 是不定长的，这里为了简单，
//   用一个"不存在的值" EMPTY 表示空节点，元素个数必须能构成完全二叉树。
//   只是为了演示，不用纠结建树细节。
// ------------------------------------------------------------
const int EMPTY = -1;

TreeNode* buildTree(const vector<int>& levelOrder, int index) {
    if (index >= (int)levelOrder.size() || levelOrder[index] == EMPTY) {
        return nullptr;
    }
    TreeNode* root = new TreeNode(levelOrder[index]);
    root->left = buildTree(levelOrder, index * 2 + 1);
    root->right = buildTree(levelOrder, index * 2 + 2);
    return root;
}

// 工具函数：后序释放整棵树
void freeTree(TreeNode* root) {
    if (root == nullptr) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// ------------------------------------------------------------
// 1. 三种递归遍历
//
// 前序：根 -> 左 -> 右（先处理自己，再把任务交给孩子）
// 中序：左 -> 根 -> 右（BST 用这个，结果升序）
// 后序：左 -> 右 -> 根（先拿到孩子结果，再算自己）
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
void preorder(TreeNode* root, vector<int>& result) {
    if (root == nullptr) return;
    result.push_back(root->value);   // 前序位置
    preorder(root->left, result);
    preorder(root->right, result);
}

void inorder(TreeNode* root, vector<int>& result) {
    if (root == nullptr) return;
    inorder(root->left, result);
    result.push_back(root->value);   // 中序位置
    inorder(root->right, result);
}

void postorder(TreeNode* root, vector<int>& result) {
    if (root == nullptr) return;
    postorder(root->left, result);
    postorder(root->right, result);
    result.push_back(root->value);   // 后序位置
}

// ------------------------------------------------------------
// 2. 迭代中序遍历（面试常问"不用递归怎么写"）
//
// 思路：
//   用一个栈模拟递归。先把当前节点和它的所有左孩子压栈，
//   弹出栈顶访问，再转向它的右孩子，重复上述过程。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
vector<int> inorderIterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> pending;
    TreeNode* current = root;

    while (current != nullptr || !pending.empty()) {
        while (current != nullptr) {      // 一路向左
            pending.push(current);
            current = current->left;
        }
        current = pending.top();          // 最左的节点出栈
        pending.pop();
        result.push_back(current->value);
        current = current->right;         // 转向右子树
    }
    return result;
}

// ------------------------------------------------------------
// 3. 层序遍历（LeetCode 102）
//
// 思路：
//   BFS + 队列。每轮先记下当前队列长度 size，这一轮只处理 size 个节点，
//   它们正好是同一层。孩子入队后留到下一轮处理。
//
// 时间复杂度 O(n)，空间复杂度 O(宽度)，最坏 O(n)。
// ------------------------------------------------------------
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (root == nullptr) return result;

    queue<TreeNode*> nodes;
    nodes.push(root);

    while (!nodes.empty()) {
        int levelSize = (int)nodes.size();  // 关键：先记下这一层的节点数
        vector<int> level;

        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = nodes.front();
            nodes.pop();
            level.push_back(node->value);
            if (node->left != nullptr) nodes.push(node->left);
            if (node->right != nullptr) nodes.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}

// ------------------------------------------------------------
// 4. 最大深度（LeetCode 104）
//
// 思路（后序）：
//   树的深度 = 1 + 左右子树深度的较大者。
//   先递归求出孩子的结果，再算自己，这是典型的后序。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
int maxDepth(TreeNode* root) {
    if (root == nullptr) return 0;
    int leftDepth = maxDepth(root->left);
    int rightDepth = maxDepth(root->right);
    return 1 + std::max(leftDepth, rightDepth);
}

// ------------------------------------------------------------
// 5. 翻转二叉树（LeetCode 226）
//
// 思路：
//   递归交换左右孩子，然后递归翻转两棵子树。空节点直接返回。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
TreeNode* invertTree(TreeNode* root) {
    if (root == nullptr) return nullptr;
    std::swap(root->left, root->right);  // 交换左右孩子
    invertTree(root->left);
    invertTree(root->right);
    return root;
}

// ------------------------------------------------------------
// 6. 对称二叉树（LeetCode 101）
//
// 思路：
//   判断两棵子树是否镜像：左子树的左孩子 vs 右子树的右孩子，
//   左子树的右孩子 vs 右子树的左孩子。用辅助函数递归比较。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
bool isMirror(TreeNode* leftTree, TreeNode* rightTree) {
    if (leftTree == nullptr && rightTree == nullptr) return true;
    if (leftTree == nullptr || rightTree == nullptr) return false;
    if (leftTree->value != rightTree->value) return false;
    return isMirror(leftTree->left, rightTree->right) &&
           isMirror(leftTree->right, rightTree->left);
}

bool isSymmetric(TreeNode* root) {
    if (root == nullptr) return true;
    return isMirror(root->left, root->right);
}

// ------------------------------------------------------------
// 7. 验证二叉搜索树（LeetCode 98）
//
// 思路（中序）：
//   BST 的中序遍历必须是严格递增的。用 prev 记上一个访问的节点，
//   如果当前值 <= prev 的值，说明不是 BST。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
//
// 易错点：只比较"父节点和直接孩子"是错的，
//   例如 5 的右孩子的左孩子可能是 4（比 5 小），必须用中序或上下界。
// ------------------------------------------------------------
bool checkBst(TreeNode* root, TreeNode*& previous) {
    if (root == nullptr) return true;

    if (!checkBst(root->left, previous)) return false;

    if (previous != nullptr && previous->value >= root->value) {
        return false;  // 中序必须严格递增
    }
    previous = root;

    return checkBst(root->right, previous);
}

bool isValidBst(TreeNode* root) {
    TreeNode* previous = nullptr;
    return checkBst(root, previous);
}

// ------------------------------------------------------------
// 8. 二叉树的最近公共祖先（LeetCode 236）
//
// 思路（后序）：
//   在左右子树里分别找 p、q：
//   - 两边都找到了：当前节点就是最近公共祖先；
//   - 只有一边找到：返回找到的那一边的结果；
//   - 自己就是 p 或 q：返回自己。
//
// 时间复杂度 O(n)，空间复杂度 O(h)。
// ------------------------------------------------------------
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || root == p || root == q) {
        return root;
    }
    TreeNode* leftResult = lowestCommonAncestor(root->left, p, q);
    TreeNode* rightResult = lowestCommonAncestor(root->right, p, q);

    if (leftResult != nullptr && rightResult != nullptr) {
        return root;  // p、q 分别在两侧，当前节点就是 LCA
    }
    return (leftResult != nullptr) ? leftResult : rightResult;
}

// ------------------------------------------------------------
// 9. 从前序与中序序列构造二叉树（LeetCode 105）
//
// 思路：
//   前序的第一个元素是根；在中序里找到这个根的位置，
//   位置左边是左子树的中序，右边是右子树的中序，
//   据此也能切出对应的前序区间，然后递归构造。
//
// 为了不用每次线性查找根的位置，先建一个"值 -> 中序下标"的哈希表
// （unordered_map）。这里特意**不用数组**做映射：节点值可能是负数，
// 也可能非常大（LeetCode 105 的范围是 -3000..3000），
// 用"值当下标"会越界或申请出巨大的数组。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
//
// 易错点：
//   - 区间边界用"左闭右闭"，长度计算别少 1；
//   - 左右子树的区间长度必须一致，切错了就会越界；
//   - 值 -> 位置要用哈希表，不能用"值当下标"的数组。
// ------------------------------------------------------------
TreeNode* buildFromPreorderInorder(
    const vector<int>& preorder, const vector<int>& inorder,
    const std::unordered_map<int, int>& position, int preLeft, int preRight,
    int inLeft, int inRight) {
    if (preLeft > preRight) return nullptr;

    int rootValue = preorder[preLeft];
    TreeNode* root = new TreeNode(rootValue);

    int rootPosition = position.at(rootValue);   // 根在中序里的位置
    int leftSize = rootPosition - inLeft;        // 左子树的节点个数

    root->left = buildFromPreorderInorder(preorder, inorder, position,
                                          preLeft + 1, preLeft + leftSize,
                                          inLeft, rootPosition - 1);
    root->right = buildFromPreorderInorder(preorder, inorder, position,
                                           preLeft + leftSize + 1, preRight,
                                           rootPosition + 1, inRight);
    return root;
}

TreeNode* buildTreeFromPreorderInorder(const vector<int>& preorder,
                                       const vector<int>& inorder) {
    if (preorder.empty()) return nullptr;

    // position[值] = 该值在中序数组里的下标（题目保证值不重复）
    // 用哈希表而不是数组：值可能是负数，也可能很大
    std::unordered_map<int, int> position;
    for (int i = 0; i < (int)inorder.size(); ++i) {
        position[inorder[i]] = i;
    }

    return buildFromPreorderInorder(preorder, inorder, position,
                                    0, (int)preorder.size() - 1,
                                    0, (int)inorder.size() - 1);
}

// 打印二维数组（用于层序结果）
void printLevels(const vector<vector<int>>& levels) {
    for (const vector<int>& level : levels) {
        cout << "  [";
        for (int i = 0; i < (int)level.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << level[i];
        }
        cout << "]" << endl;
    }
}

int main() {
    // 建一棵示例树（层序数组 [1, 2, 3, 4, 5, 空, 6]）：
    // 根是 1；1 的左右孩子是 2 和 3；
    // 2 的左右孩子是 4 和 5；3 的右孩子是 6（左孩子为空）。
    vector<int> levelValues = {1, 2, 3, 4, 5, EMPTY, 6};
    TreeNode* root = buildTree(levelValues, 0);

    // 示例 1：三种遍历
    vector<int> pre, in, post;
    preorder(root, pre);
    inorder(root, in);
    postorder(root, post);
    cout << "前序：";
    for (int value : pre) cout << value << " ";
    cout << endl;
    cout << "中序：";
    for (int value : in) cout << value << " ";
    cout << endl;
    cout << "后序：";
    for (int value : post) cout << value << " ";
    cout << endl;
    assert((pre == vector<int>{1, 2, 4, 5, 3, 6}));
    assert((in == vector<int>{4, 2, 5, 1, 3, 6}));
    assert((post == vector<int>{4, 5, 2, 6, 3, 1}));

    // 示例 2：迭代中序和递归中序结果必须一致
    assert(inorderIterative(root) == in);
    cout << "迭代中序与递归中序结果一致" << endl;

    // 示例 3：层序遍历
    cout << "层序遍历：" << endl;
    vector<vector<int>> levels = levelOrder(root);
    printLevels(levels);
    assert((levels == vector<vector<int>>{{1}, {2, 3}, {4, 5, 6}}));

    // 示例 4：最大深度、翻转、对称
    assert(maxDepth(root) == 3);
    invertTree(root);
    vector<int> invertedIn;
    inorder(root, invertedIn);
    assert((invertedIn == vector<int>{6, 3, 1, 5, 2, 4}));  // 中序变成降序
    assert(isSymmetric(root) == false);
    invertTree(root);  // 翻回去
    cout << "最大深度 = 3，翻转后中序逆序，非对称树判定正确" << endl;

    // 示例 5：验证 BST
    vector<int> bstLevel = {5, 3, 8, 1, 4, EMPTY, 9};
    TreeNode* bst = buildTree(bstLevel, 0);
    assert(isValidBst(bst) == true);
    bst->left->right->value = 6;  // 人为破坏：5 的左子树里出现 6
    assert(isValidBst(bst) == false);
    cout << "验证 BST：正确树通过，破坏后判为非法" << endl;

    // 示例 6：最近公共祖先
    // 在 bst（值已改动）上不用管，重新建一棵干净的树
    TreeNode* lcaTree = buildTree({3, 5, 1, 6, 2, 0, 8}, 0);
    TreeNode* node5 = lcaTree->left;
    TreeNode* node1 = lcaTree->right;
    assert(lowestCommonAncestor(lcaTree, node5, node1) == lcaTree);
    assert(lowestCommonAncestor(lcaTree, node5, node5->right) == node5);
    cout << "最近公共祖先：5 和 1 的 LCA = " << lcaTree->value
         << "，5 和 2 的 LCA = " << node5->value << endl;

    // 示例 7：从前序 + 中序建树（节点值可以是负数）
    vector<int> preorderInput = {3, 9, 20, 15, 7};
    vector<int> inorderInput = {9, 3, 15, 20, 7};
    TreeNode* rebuilt = buildTreeFromPreorderInorder(preorderInput, inorderInput);
    assert(rebuilt != nullptr && rebuilt->value == 3);
    assert(rebuilt->left->value == 9);
    assert(rebuilt->right->value == 20);
    vector<int> rebuiltPre, rebuiltIn;
    preorder(rebuilt, rebuiltPre);
    inorder(rebuilt, rebuiltIn);
    assert(rebuiltPre == preorderInput);
    assert(rebuiltIn == inorderInput);
    cout << "前序 + 中序建树：重建后的遍历结果与原输入一致" << endl;

    // 含负数节点值也要能建树（值 -> 位置必须用哈希表，不能用数组下标）
    vector<int> negativePreorder = {0, -5, -10, 3, -2};
    vector<int> negativeInorder = {-10, -5, 0, 3, -2};
    TreeNode* negativeTree =
        buildTreeFromPreorderInorder(negativePreorder, negativeInorder);
    vector<int> negativeRebuiltPre;
    preorder(negativeTree, negativeRebuiltPre);
    assert(negativeRebuiltPre == negativePreorder);
    cout << "前序 + 中序建树：负数节点值也能正常处理" << endl;

    // 释放内存
    freeTree(root);
    freeTree(bst);
    freeTree(lcaTree);
    freeTree(rebuilt);
    freeTree(negativeTree);

    // 边界：空树
    assert(maxDepth(nullptr) == 0);
    assert(isValidBst(nullptr) == true);
    assert(levelOrder(nullptr).empty());
    assert(isSymmetric(nullptr) == true);

    cout << "07 二叉树：全部示例通过" << endl;
    return 0;
}
