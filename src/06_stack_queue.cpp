// ============================================================
// 专题 06：栈、队列、单调栈、单调队列
//
// 适用题型：有效括号、最小栈、每日温度、下一个更大元素、
//           柱状图中最大的矩形、接雨水、滑动窗口最大值。
//
// 核心思想：
//   - 栈（stack）：后进先出，适合"最近的一对""撤销""嵌套结构"；
//   - 队列（queue）：先进先出，适合"按顺序处理"；
//   - 单调栈：栈里保持单调递增或递减，每个元素只进栈、出栈各一次，
//     所以 O(n) 就能求出"每个元素左边/右边第一个比它大/小的元素"；
//   - 单调队列：队列里保持单调递减，队首就是当前窗口最大值，
//     用来把"滑动窗口最大值"从 O(n log n) 降到 O(n)。
//
// 时间复杂度：每个函数都是 O(n)
// 空间复杂度：O(n)
//
// 易错点：
//   1. 单调栈里弹栈要用 while 而不是 if，可能一次弹出多个。
//   2. 栈里存"下标"还是"值"要看题目：需要算距离就存下标。
//   3. 取栈顶 / 队首前必须判断非空，否则是未定义行为。
//   4. 单调队列里出队时，要判断队首是否已经滑出窗口。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/06_stack_queue.cpp -o /tmp/demo06
//   /tmp/demo06
// ============================================================

#include <cassert>
#include <deque>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using std::cout;
using std::deque;
using std::endl;
using std::stack;
using std::string;
using std::vector;

// ------------------------------------------------------------
// 题目 1：有效的括号（LeetCode 20）
//
// 思路：
//   遇到左括号就压栈；遇到右括号就看栈顶是不是与它配对的那个左括号。
//   最后栈必须为空，否则说明有左括号没被匹配。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
// ------------------------------------------------------------
bool isValidParentheses(const string& text) {
    stack<char> brackets;

    for (char c : text) {
        if (c == '(' || c == '[' || c == '{') {
            brackets.push(c);
            continue;
        }
        if (brackets.empty()) {
            return false;  // 右括号多余
        }
        char top = brackets.top();
        brackets.pop();
        if ((c == ')' && top != '(') ||
            (c == ']' && top != '[') ||
            (c == '}' && top != '{')) {
            return false;  // 括号类型不匹配
        }
    }
    return brackets.empty();  // 左括号必须全部用完
}

// ------------------------------------------------------------
// 题目 2：最小栈（LeetCode 155）—— 辅助栈
//
// 思路：
//   除了正常保存数据的栈，再开一个"最小值栈"，
//   每次入栈时把"当前最小值"也压进最小值栈。
//   这样 pop 之后最小值栈的栈顶仍然是剩余元素的最小值。
//
// 时间复杂度：每个操作 O(1)，空间复杂度 O(n)。
// ------------------------------------------------------------
class MinStack {
public:
    void push(int value) {
        values.push(value);
        if (minimums.empty()) {
            minimums.push(value);
        } else {
            minimums.push(std::min(value, minimums.top()));
        }
    }

    void pop() {
        values.pop();
        minimums.pop();
    }

    int top() const { return values.top(); }

    int getMin() const { return minimums.top(); }

private:
    stack<int> values;
    stack<int> minimums;
};

// ------------------------------------------------------------
// 题目 3：下一个更大元素 / 每日温度（LeetCode 739）—— 单调栈
//
// 思路：
//   栈里存"还没找到下一个更大值的那些下标"，栈内对应的值单调递减。
//   遍历到 i 时，如果 a[i] 比栈顶元素大，说明栈顶元素的下一个更大值
//   就是 a[i]，弹出并记录；然后把自己压栈。
//
// 为什么是 O(n)？每个下标最多进栈一次、出栈一次。
// 返回值：answer[i] = 下一个更大值的下标，没有则为 -1。
// ------------------------------------------------------------
vector<int> nextGreaterElement(const vector<int>& numbers) {
    int n = (int)numbers.size();
    vector<int> answer(n, -1);
    stack<int> indexStack;  // 存下标，方便算距离

    for (int i = 0; i < n; ++i) {
        while (!indexStack.empty() && numbers[indexStack.top()] < numbers[i]) {
            int previous = indexStack.top();
            indexStack.pop();
            answer[previous] = i;  // 找到第一个更大的，就是 i
        }
        indexStack.push(i);
    }
    return answer;
}

// ------------------------------------------------------------
// 题目 4：每日温度（LeetCode 739）—— 单调栈求距离
//
// 思路：和上一题一样，只是答案存"等了几天"而不是下标。
// 时间复杂度 O(n)，空间复杂度 O(n)。
// ------------------------------------------------------------
vector<int> dailyTemperatures(const vector<int>& temperatures) {
    int n = (int)temperatures.size();
    vector<int> answer(n, 0);
    stack<int> indexStack;

    for (int i = 0; i < n; ++i) {
        while (!indexStack.empty() &&
               temperatures[indexStack.top()] < temperatures[i]) {
            int previous = indexStack.top();
            indexStack.pop();
            answer[previous] = i - previous;
        }
        indexStack.push(i);
    }
    return answer;  // 后面没有更高温度的仍然是 0
}

// ------------------------------------------------------------
// 题目 5：滑动窗口最大值（LeetCode 239）—— 单调队列
//
// 思路：
//   用双端队列存下标，队列里的值从队首到队尾单调递减。
//   1. 新元素入队前，把队尾所有比它小的都弹掉（它们永远不会成为答案）；
//   2. 检查队首是否已经滑出窗口，滑出就弹出；
//   3. 窗口长度到达 k 后，队首就是当前窗口最大值。
//
// 时间复杂度 O(n)（每个元素最多进出队列一次），空间复杂度 O(k)。
// ------------------------------------------------------------
vector<int> maxSlidingWindow(const vector<int>& numbers, int k) {
    vector<int> answer;
    deque<int> indexDeque;  // 存下标，对应值从大到小

    for (int i = 0; i < (int)numbers.size(); ++i) {
        // 1. 队尾比当前值小的都没用了
        while (!indexDeque.empty() && numbers[indexDeque.back()] <= numbers[i]) {
            indexDeque.pop_back();
        }
        indexDeque.push_back(i);

        // 2. 队首元素已经滑出窗口 [i-k+1, i]
        if (indexDeque.front() <= i - k) {
            indexDeque.pop_front();
        }

        // 3. 窗口成形后，队首就是最大值
        if (i >= k - 1) {
            answer.push_back(numbers[indexDeque.front()]);
        }
    }
    return answer;
}

// ------------------------------------------------------------
// 题目 6：柱状图中最大的矩形（LeetCode 84）—— 单调栈 + 哨兵
//
// 思路：
//   枚举"以某根柱子为高度"能向左右扩展到多远：
//   找到左边第一个比它矮的、右边第一个比它矮的，中间就是可扩展范围。
//   在数组两端各加一个高度 0 的哨兵，这样所有柱子最后都会被弹出，
//   不用在循环结束后再补一次处理。
//
// 时间复杂度 O(n)，空间复杂度 O(n)。
// ------------------------------------------------------------
long long largestRectangleArea(const vector<int>& heights) {
    vector<int> withSentinel;
    withSentinel.push_back(0);  // 左哨兵
    for (int h : heights) withSentinel.push_back(h);
    withSentinel.push_back(0);  // 右哨兵

    long long best = 0;
    stack<int> indexStack;

    for (int i = 0; i < (int)withSentinel.size(); ++i) {
        while (!indexStack.empty() &&
               withSentinel[indexStack.top()] > withSentinel[i]) {
            int height = withSentinel[indexStack.top()];
            indexStack.pop();
            int left = indexStack.empty() ? -1 : indexStack.top();
            long long width = i - left - 1;
            best = std::max(best, height * width);
        }
        indexStack.push(i);
    }
    return best;
}

// ------------------------------------------------------------
// 题目 7：用两个栈实现队列（LeetCode 232）
//
// 思路：
//   - inStack 只管入队，直接压栈；
//   - outStack 只管出队，为空时把 inStack 里的元素全部倒过来。
//   每个元素最多被"倒"一次，所以均摊时间复杂度 O(1)。
//
// 时间复杂度：push O(1)，pop / peek 均摊 O(1)；空间复杂度 O(n)。
// ------------------------------------------------------------
class QueueWithTwoStacks {
public:
    void push(int value) { inStack.push(value); }

    int pop() {
        moveInToOut();
        int value = outStack.top();
        outStack.pop();
        return value;
    }

    int front() {
        moveInToOut();
        return outStack.top();
    }

    bool empty() const { return inStack.empty() && outStack.empty(); }

private:
    void moveInToOut() {
        if (!outStack.empty()) return;  // 出栈里还有，直接复用
        while (!inStack.empty()) {
            outStack.push(inStack.top());
            inStack.pop();
        }
    }

    stack<int> inStack;   // 负责入队
    stack<int> outStack;  // 负责出队
};

int main() {
    // 示例 1：有效括号
    assert(isValidParentheses("()[]{}") == true);
    assert(isValidParentheses("(]") == false);
    assert(isValidParentheses("([)]") == false);
    assert(isValidParentheses("(") == false);
    assert(isValidParentheses("") == true);
    cout << "有效括号：\"()[]{}\" -> 合法，(] -> 不合法，([)] -> 不合法" << endl;

    // 示例 2：最小栈
    MinStack minStack;
    minStack.push(-2);
    minStack.push(0);
    minStack.push(-3);
    assert(minStack.getMin() == -3);
    minStack.pop();
    assert(minStack.top() == 0);
    assert(minStack.getMin() == -2);
    cout << "最小栈：push(-2, 0, -3) 后最小值 = -3，pop 后最小值 = -2" << endl;

    // 示例 3：下一个更大元素
    vector<int> numbers = {2, 1, 2, 4, 3};
    vector<int> answer = nextGreaterElement(numbers);
    cout << "下一个更大元素：数组 [2, 1, 2, 4, 3] -> 下标 [";
    for (int i = 0; i < (int)answer.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << answer[i];
    }
    cout << "]" << endl;
    assert((answer == vector<int>{3, 2, 3, -1, -1}));

    // 示例 4：每日温度
    vector<int> temperatures = {73, 74, 75, 71, 69, 72, 76, 73};
    vector<int> waits = dailyTemperatures(temperatures);
    cout << "每日温度：[73, 74, 75, 71, 69, 72, 76, 73] -> [";
    for (int i = 0; i < (int)waits.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << waits[i];
    }
    cout << "]" << endl;
    assert((waits == vector<int>{1, 1, 4, 2, 1, 1, 0, 0}));

    // 示例 5：滑动窗口最大值
    vector<int> windowAnswer = maxSlidingWindow({1, 3, -1, -3, 5, 3, 6, 7}, 3);
    cout << "滑动窗口最大值：[1, 3, -1, -3, 5, 3, 6, 7]，k = 3 -> [";
    for (int i = 0; i < (int)windowAnswer.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << windowAnswer[i];
    }
    cout << "]" << endl;
    assert((windowAnswer == vector<int>{3, 3, 5, 5, 6, 7}));

    // 示例 7：两个栈实现队列
    QueueWithTwoStacks queue;
    queue.push(1);
    queue.push(2);
    int firstOut = queue.front();
    int secondOut = queue.pop();
    cout << "两个栈实现队列：push(1) push(2) 后 front = " << firstOut
         << "，pop = " << secondOut << "，空队列 = "
         << (queue.empty() ? "是" : "否") << endl;
    assert(firstOut == 1);
    assert(secondOut == 1);
    queue.pop();  // 把剩下的 2 也弹掉
    assert(queue.empty() == true);

    // 示例 6：柱状图最大矩形
    long long area = largestRectangleArea({2, 1, 5, 6, 2, 3});
    cout << "柱状图最大矩形：[2, 1, 5, 6, 2, 3] -> 面积 = " << area << endl;
    assert(area == 10);
    assert(largestRectangleArea({1}) == 1);
    assert(largestRectangleArea({}) == 0);

    cout << "06 栈与单调栈：全部示例通过" << endl;
    return 0;
}
