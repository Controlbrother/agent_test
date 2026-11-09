// ============================================================
// 专题 01：双指针（对撞指针）
//
// 适用题型：有序数组两数之和、三数之和、盛最多水的容器、
//           验证回文串、合并两个有序数组。
//
// 核心思想：
//   用两个下标 left、right 从数组两端向中间移动，
//   每一步都根据当前情况丢掉一个不可能的答案，
//   把 O(n^2) 的枚举压缩成 O(n)。
//
// 时间复杂度：O(n)（三数之和是排序 O(n log n) + 枚举 O(n^2)）
// 空间复杂度：O(1)（不算保存答案的空间）
//
// 易错点：
//   1. while (left < right) 不要写成 <=，否则同一个元素会被用两次。
//   2. 移动指针的条件要和"数组有序"对应：和太小就左指针右移，
//      和太大就右指针左移，不能反。
//   3. 三数之和要去重：外层跳过重复数字，内层指针移动后也要跳过。
//
// 编译运行：
//   g++ -std=c++17 -Wall -Wextra -O2 src/01_two_pointers.cpp -o /tmp/demo01
//   /tmp/demo01
// ============================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// 打印一维数组，例如 [1, 2, 3]
void printVector(const vector<int>& nums) {
    cout << "[";
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << nums[i];
    }
    cout << "]";
}

// 打印二维数组，每个三元组一行
void printTriplets(const vector<vector<int>>& triplets) {
    for (const vector<int>& one : triplets) {
        printVector(one);
        cout << " ";
    }
    cout << endl;
}

// ------------------------------------------------------------
// 题目 1：有序数组两数之和（LeetCode 167）
//
// 思路：
//   数组已经从小到大排好序。left 指向最小值，right 指向最大值。
//   如果两数之和小于 target，说明 left 太小，只能右移 left；
//   如果两数之和大于 target，说明 right 太大，只能左移 right。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// 返回值：找到时返回两个下标（从 1 开始），找不到返回 {-1, -1}。
// ------------------------------------------------------------
std::pair<int, int> twoSumSorted(const vector<int>& nums, int target) {
    int left = 0;
    int right = (int)nums.size() - 1;

    while (left < right) {
        int sum = nums[left] + nums[right];
        if (sum == target) {
            return {left + 1, right + 1};  // 题目要求下标从 1 开始
        }
        if (sum < target) {
            ++left;   // 和太小，让小的数变大
        } else {
            --right;  // 和太大，让大的数变小
        }
    }
    return {-1, -1};
}

// ------------------------------------------------------------
// 题目 2：三数之和（LeetCode 15）
//
// 思路：
//   1. 先排序，方便去重和使用双指针。
//   2. 固定第一个数 nums[i]，问题变成"在 i 后面找两数之和 = -nums[i]"。
//   3. 用 left、right 对撞指针找另外两个数。
//
// 时间复杂度 O(n^2)，空间复杂度 O(1)（不算答案）。
//
// 易错点：
//   - 外层 i 要跳过重复数字，否则答案会重复。
//   - 找到一个答案后，left 和 right 都要跳过重复值。
// ------------------------------------------------------------
vector<vector<int>> threeSum(vector<int> nums) {
    vector<vector<int>> result;
    std::sort(nums.begin(), nums.end());
    int n = (int)nums.size();

    for (int i = 0; i + 2 < n; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) {
            continue;  // 第一个数重复，跳过，避免重复三元组
        }

        int left = i + 1;
        int right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});

                // 左右指针都跳过重复值，再继续找下一组
                ++left;
                --right;
                while (left < right && nums[left] == nums[left - 1]) ++left;
                while (left < right && nums[right] == nums[right + 1]) --right;
            } else if (sum < 0) {
                ++left;   // 和太小，左指针右移
            } else {
                --right;  // 和太大，右指针左移
            }
        }
    }
    return result;
}

// ------------------------------------------------------------
// 题目 3：盛最多水的容器（LeetCode 11）
//
// 思路：
//   容器的容量 = 两个指针的距离 × 两条边中较短的那条。
//   每次移动较短的那条边：因为移动长边不会让高度变高，
//   宽度却一定变小，所以不可能得到更大的容量。
//
// 时间复杂度 O(n)，空间复杂度 O(1)。
// ------------------------------------------------------------
int maxArea(const vector<int>& height) {
    int left = 0;
    int right = (int)height.size() - 1;
    int best = 0;

    while (left < right) {
        int h = std::min(height[left], height[right]);
        int width = right - left;
        best = std::max(best, h * width);

        // 移动较矮的一边，才有可能遇到更高的边
        if (height[left] < height[right]) {
            ++left;
        } else {
            --right;
        }
    }
    return best;
}

int main() {
    // 示例 1：有序数组两数之和
    vector<int> sortedNums = {2, 7, 11, 15};
    std::pair<int, int> ans1 = twoSumSorted(sortedNums, 9);
    cout << "两数之和：nums = ";
    printVector(sortedNums);
    cout << ", target = 9，答案下标 = [" << ans1.first << ", " << ans1.second << "]" << endl;
    assert(ans1.first == 1 && ans1.second == 2);

    // 示例 2：三数之和
    vector<int> nums = {-1, 0, 1, 2, -1, -4};
    vector<vector<int>> ans2 = threeSum(nums);
    cout << "三数之和：nums = ";
    printVector(nums);
    cout << "，答案 = ";
    printTriplets(ans2);
    assert((int)ans2.size() == 2);
    assert((ans2[0] == vector<int>{-1, -1, 2}));
    assert((ans2[1] == vector<int>{-1, 0, 1}));

    // 示例 3：盛最多水的容器
    vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    int ans3 = maxArea(heights);
    cout << "盛水容器：heights = ";
    printVector(heights);
    cout << "，最大容量 = " << ans3 << endl;
    assert(ans3 == 49);

    cout << "01 双指针：全部示例通过" << endl;
    return 0;
}
