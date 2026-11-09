#!/usr/bin/env bash
# ============================================================
# 自动化测试入口（build_all.sh 的"考试版"）
#
# 和 build_all.sh 的区别：
#   - 这个脚本用于交作业 / 提交前自查，额外加了：
#       * -Werror：警告直接当错误，杜绝"能跑但有一堆 warning"；
#       * 输出对比用 diff，能看清具体哪一行不一样；
#       * 最后打印 PASS / FAIL 汇总，方便贴到 PR 或笔记里。
#
# 用法（在仓库根目录执行）：
#   ./tests/run_tests.sh
# ============================================================

set -u

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT" || exit 1

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:--std=c++17 -Wall -Wextra -Werror -O2}"

BUILD_DIR="build/tests"
mkdir -p "$BUILD_DIR"

passed=0
failed=0
failed_names=""

for source in src/*.cpp; do
    name="$(basename "$source" .cpp)"

    # 1. 编译（警告即错误）
    if ! "$CXX" $CXXFLAGS -o "$BUILD_DIR/$name" "$source"; then
        echo "FAIL $name : 编译失败或有警告"
        failed=$((failed + 1))
        failed_names="$failed_names $name"
        continue
    fi

    # 2. 运行并校验
    sample_input="tests/samples/$name.in"
    expected_output="tests/expected/$name.out"

    if [ -f "$sample_input" ] && [ -f "$expected_output" ]; then
        actual_file="$BUILD_DIR/$name.actual"
        if ! "$BUILD_DIR/$name" < "$sample_input" > "$actual_file" 2>&1; then
            echo "FAIL $name : 运行失败（退出码非 0）"
            failed=$((failed + 1))
            failed_names="$failed_names $name"
            continue
        fi
        if ! diff -u "$expected_output" "$actual_file"; then
            echo "FAIL $name : 输出与期望不一致"
            failed=$((failed + 1))
            failed_names="$failed_names $name"
            continue
        fi
    else
        actual_file="$BUILD_DIR/$name.actual"
        if ! "$BUILD_DIR/$name" > "$actual_file" 2>&1; then
            echo "FAIL $name : 运行失败（assert 可能没通过）"
            failed=$((failed + 1))
            failed_names="$failed_names $name"
            continue
        fi
        if ! grep -q "全部示例通过" "$actual_file"; then
            echo "FAIL $name : 没有打印'全部示例通过'"
            failed=$((failed + 1))
            failed_names="$failed_names $name"
            continue
        fi
    fi

    echo "PASS $name"
    passed=$((passed + 1))
done

echo
echo "通过 $passed 个，失败 $failed 个"
if [ $failed -ne 0 ]; then
    echo "失败项:$failed_names"
    exit 1
fi
echo "全部测试通过"
