// ============================================================
// 专题 15：排序
//
// 适用题型：手写快排 / 归并排序、排序链表、
//           第 K 大元素（快速选择）、合并区间、自定义排序（最大数）。
//
// 核心思想：
//   面试要求手写的只有两个：快排和归并。其他场景直接 std::sort。
//   - 快速排序：选一个基准，把小的放左边、大的放右边，再递归两边；
//   - 归并排序：把数组分成两半分别排好，再合并成一个有序数组；
//     合并时顺便能统计逆序对。
//
// 时间复杂度：
//   快排 平均 O(n log n)，最坏 O(n^2)（基准选得不好时）
//   归并 稳定 O(n log n)
// 空间复杂度：
//   快排 O(log n)（递归栈），归并 O(n)（临时数组）
//
// 易错点：
//   1. 快排基准取"首元素"在有序数据上会退化 O(n^2)，
//      要取中间元素或随机元素。
//   2. std::sort 的比较器必须满足严格弱序（不能写 <=），
//      否则是未定义行为，可能直接段错误。
//   3. 归并排序的临时数组在最外层分配一次传进去，
//      不要在递归里每次新建 vector。
//   4. 快速选择只递归一半，平均 O(n)，注意划分的边界。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/15_sorting.cpp -o /tmp/demo15
//   /tmp/demo15
// ============================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// ------------------------------------------------------------
// 1. 快速排序（区间是闭区间 [left, right]）
//
// 思路：
//   1. 取中间元素作为基准 pivot；
//   2. 用 i、j 从两端向中间扫，把小于基准的放左边、大于的放右边；
//   3. 基准归位后，递归排序左右两段。
//
// 时间复杂度 平均 O(n log n)，空间复杂度 O(log n)。
// ------------------------------------------------------------
void quickSort(vector<int>& numbers, int left, int right) {
    if (left >= right) return;  // 0 个或 1 个元素，不用排

    int pivot = numbers[left + (right - left) / 2];  // 取中间值防退化
    int i = left;
    int j = right;

    while (i <= j) {
        while (numbers[i] < pivot) ++i;   // 左边找 >= pivot 的
        while (numbers[j] > pivot) --j;   // 右边找 <= pivot 的
        if (i <= j) {
            std::swap(numbers[i], numbers[j]);
            ++i;
            --j;
        }
    }

    // 此时 [left, j] 都 <= pivot，[i, right] 都 >= pivot
    quickSort(numbers, left, j);
    quickSort(numbers, i, right);
}

// ------------------------------------------------------------
// 2. 归并排序（区间是闭区间 [left, right]），返回逆序对个数
//
// 思路：
//   1. 分：从中间切成两半，分别递归排序；
//   2. 治：把两个有序段合并到临时数组 temp，再拷回原数组。
//   合并时如果右边元素先被选走，说明它比左边剩下的所有元素都小，
//   因此逆序对数 += 左边剩余元素个数。
//
// 时间复杂度 O(n log n)，空间复杂度 O(n)。
// ------------------------------------------------------------
long long mergeSort(vector<int>& numbers, vector<int>& temp, int left,
                    int right) {
    if (left >= right) return 0;

    int middle = left + (right - left) / 2;
    long long inversions = mergeSort(numbers, temp, left, middle) +
                           mergeSort(numbers, temp, middle + 1, right);

    int i = left;
    int j = middle + 1;
    int k = left;

    while (i <= middle && j <= right) {
        if (numbers[i] <= numbers[j]) {
            temp[k++] = numbers[i++];
        } else {
            temp[k++] = numbers[j++];
            inversions += middle - i + 1;  // 左边剩下的都比它大
        }
    }
    while (i <= middle) temp[k++] = numbers[i++];
    while (j <= right) temp[k++] = numbers[j++];
    for (int t = left; t <= right; ++t) numbers[t] = temp[t];

    return inversions;
}

// ------------------------------------------------------------
// 3. 快速选择：求第 k 大的元素（LeetCode 215），平均 O(n)
//
// 思路：
//   快排的划分过程会把基准放到最终位置上。
//   如果基准的位置正好是"第 k 大"应该在的位置，就直接返回；
//   否则只递归有答案的那一半（另一半完全不用管）。
//
// 时间复杂度 平均 O(n)，最坏 O(n^2)；空间复杂度 O(1)（迭代写法）。
//
// 注意：这里按"从大到小"的思路找：第 k 大 = 升序下标 n - k。
// ------------------------------------------------------------
int kthLargestByQuickSelect(vector<int> numbers, int k) {
    int targetIndex = (int)numbers.size() - k;  // 升序后第 k 大所在下标
    int left = 0;
    int right = (int)numbers.size() - 1;

    while (true) {
        int pivot = numbers[left + (right - left) / 2];
        int i = left;
        int j = right;
        while (i <= j) {
            while (numbers[i] < pivot) ++i;
            while (numbers[j] > pivot) --j;
            if (i <= j) {
                std::swap(numbers[i], numbers[j]);
                ++i;
                --j;
            }
        }
        // 划分后：[left, j] <= pivot <= [i, right]
        if (targetIndex <= j) {
            right = j;          // 答案在左半边
        } else if (targetIndex >= i) {
            left = i;           // 答案在右半边
        } else {
            return numbers[targetIndex];  // 正好落在 pivot 区间
        }
    }
}

// ------------------------------------------------------------
// 4. 自定义排序：把数组排成最小的数（剑指 Offer 45 / LeetCode 179 的反向）
//
// 思路：
//   如果 a + b < b + a，那么 a 应该排在 b 前面。
//   按这个规则排序后拼起来就是最小的数。
//
// 时间复杂度 O(n log n * m)，m 是数字位数。
// 易错点：比较器不能写 <=（不是严格弱序）。
// ------------------------------------------------------------
string smallestConcatenation(const vector<int>& numbers) {
    vector<string> parts;
    for (int value : numbers) parts.push_back(std::to_string(value));

    std::sort(parts.begin(), parts.end(), [](const string& a, const string& b) {
        return a + b < b + a;  // 注意是 <，不能是 <=
    });

    string result;
    for (const string& part : parts) result += part;
    return result;
}

// ------------------------------------------------------------
// 5. 按自定义规则排序：结构体排序（面试高频）
//
// 思路：
//   给 std::sort 传一个 lambda 比较器即可。
//   规则：先按分数从高到低；分数相同时按名字字典序升序。
//
// 时间复杂度 O(n log n)。
// ------------------------------------------------------------
struct Student {
    string name;
    int score;
};

vector<Student> sortStudents(vector<Student> students) {
    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) {
                  if (a.score != b.score) return a.score > b.score;  // 分高在前
                  return a.name < b.name;                            // 同分按名字
              });
    return students;
}

// 工具函数：打印数组
void printNumbers(const vector<int>& numbers) {
    cout << "[";
    for (int i = 0; i < (int)numbers.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << numbers[i];
    }
    cout << "]";
}

int main() {
    // 示例 1：快速排序
    vector<int> numbers1 = {5, 2, 9, 1, 5, 6, 3};
    vector<int> expected1 = numbers1;
    std::sort(expected1.begin(), expected1.end());

    vector<int> forQuickSort = numbers1;
    quickSort(forQuickSort, 0, (int)forQuickSort.size() - 1);
    cout << "快速排序：";
    printNumbers(numbers1);
    cout << " -> ";
    printNumbers(forQuickSort);
    cout << endl;
    assert(forQuickSort == expected1);

    // 示例 2：归并排序 + 逆序对
    vector<int> numbers2 = {7, 5, 6, 4};
    vector<int> temp(numbers2.size(), 0);
    vector<int> expected2 = numbers2;
    std::sort(expected2.begin(), expected2.end());
    long long inversions = mergeSort(numbers2, temp, 0, (int)numbers2.size() - 1);
    cout << "归并排序：[7, 5, 6, 4] -> ";
    printNumbers(numbers2);
    cout << "，逆序对 = " << inversions << endl;
    assert(numbers2 == expected2);
    assert(inversions == 5);  // (7,5) (7,6) (7,4) (5,4) (6,4)

    // 示例 3：快速选择
    vector<int> numbers3 = {3, 2, 1, 5, 6, 4};
    cout << "快速选择：[3, 2, 1, 5, 6, 4] 第 2 大 = "
         << kthLargestByQuickSelect(numbers3, 2) << endl;
    assert(kthLargestByQuickSelect(numbers3, 2) == 5);
    assert(kthLargestByQuickSelect({1}, 1) == 1);

    // 示例 4：自定义排序（拼成最小的数）
    cout << "拼成最小的数：[10, 2, 3, 30] -> "
         << smallestConcatenation({10, 2, 3, 30}) << endl;
    assert(smallestConcatenation({10, 2, 3, 30}) == "102303");
    assert(smallestConcatenation({3, 30, 34, 5, 9}) == "3033459");

    // 示例 5：结构体排序
    vector<Student> students = {
        {"小明", 90}, {"小红", 95}, {"小刚", 90}, {"小丽", 80}};
    vector<Student> sorted = sortStudents(students);
    cout << "结构体排序（分数降序，同分按名字）：";
    for (const Student& student : sorted) {
        cout << student.name << "(" << student.score << ") ";
    }
    cout << endl;
    assert(sorted[0].name == "小红");
    assert(sorted[1].name == "小刚");  // 同分 90，名字字典序 "小刚" < "小明"
    assert(sorted[2].name == "小明");

    // 示例 6：std::sort 基本用法
    vector<int> numbers4 = {4, 1, 3, 2};
    std::sort(numbers4.begin(), numbers4.end());                 // 升序
    assert((numbers4 == vector<int>{1, 2, 3, 4}));
    std::sort(numbers4.begin(), numbers4.end(), std::greater<int>());  // 降序
    assert((numbers4 == vector<int>{4, 3, 2, 1}));

    // 边界：空数组、单元素、全部相同
    vector<int> empty;
    quickSort(empty, 0, -1);
    assert(empty.empty());
    vector<int> single = {42};
    quickSort(single, 0, 0);
    assert(single[0] == 42);
    vector<int> same(5, 7);
    vector<int> tempSame(5, 0);
    assert(mergeSort(same, tempSame, 0, 4) == 0);  // 没有逆序对
    cout << "std::sort 升序 / 降序以及空数组、单元素、全相同边界都通过" << endl;

    cout << "15 排序：全部示例通过" << endl;
    return 0;
}
