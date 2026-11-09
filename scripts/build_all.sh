#!/usr/bin/env bash
# ============================================================
# 编译并运行全部练习代码
#
# 用法（在仓库根目录执行）：
#   ./scripts/build_all.sh
#
# 这个脚本做三件事：
#   1. 用 g++ -std=c++17 -Wall -Wextra 编译 src/ 下每一个 .cpp，
#      有任何一个 warning 都算失败（不是靠眼睛看）；
#   2. 运行每个程序，检查退出码为 0；
#   3. 对 00_basics.cpp 这种"从标准输入读数据"的程序，
#      用 tests/samples/ 里的样例输入，和 tests/expected/ 对比输出。
#
# 前提：安装了 g++（支持 C++17 即可，Ubuntu 22.04 自带的 11.4 就行）。
# ============================================================

set -u  # 使用到未定义变量时报错

# 切换到脚本所在目录的上一级，也就是仓库根目录
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT" || exit 1

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:--std=c++17 -Wall -Wextra -O2}"

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

total=0
failed=0

echo "==================== 编译 ===================="
for source in src/*.cpp; do
    name="$(basename "$source" .cpp)"
    binary="$BUILD_DIR/$name"
    total=$((total + 1))

    # 把编译输出同时收集起来，用来判断有没有 warning
    compile_log="$("$CXX" $CXXFLAGS -o "$binary" "$source" 2>&1)"
    status=$?

    if [ $status -ne 0 ]; then
        echo "[编译失败] $source"
        echo "$compile_log"
        failed=$((failed + 1))
        continue
    fi
    if [ -n "$compile_log" ]; then
        echo "[有警告] $source"
        echo "$compile_log"
        failed=$((failed + 1))
        continue
    fi
    echo "[编译通过] $source"
done

echo
echo "==================== 运行 ===================="
for source in src/*.cpp; do
    name="$(basename "$source" .cpp)"
    binary="$BUILD_DIR/$name"
    [ -x "$binary" ] || continue

    sample_input="tests/samples/$name.in"
    expected_output="tests/expected/$name.out"

    if [ -f "$sample_input" ]; then
        # 有样例输入：重定向输入，把输出和期望结果对比
        actual_output="$("$binary" < "$sample_input" 2>&1)"
        if [ $? -ne 0 ]; then
            echo "[运行失败] $name（退出码非 0）"
            failed=$((failed + 1))
            continue
        fi
        if [ -f "$expected_output" ]; then
            expected="$(cat "$expected_output")"
            if [ "$actual_output" != "$expected" ]; then
                echo "[输出不一致] $name"
                echo "--- 期望 ---"
                echo "$expected"
                echo "--- 实际 ---"
                echo "$actual_output"
                failed=$((failed + 1))
                continue
            fi
            echo "[样例通过] $name"
        else
            echo "[运行通过] $name（无期望输出文件）"
        fi
    else
        # 没有样例输入：程序自带 assert 和最后一行"全部示例通过"
        actual_output="$("$binary" 2>&1)"
        if [ $? -ne 0 ]; then
            echo "[运行失败] $name（退出码非 0，assert 可能失败）"
            echo "$actual_output"
            failed=$((failed + 1))
            continue
        fi
        echo "$actual_output" | tail -n 1
        if ! echo "$actual_output" | grep -q "全部示例通过"; then
            echo "[提示] $name 没有输出'全部示例通过'，请确认它是否自带示例"
        fi
    fi
done

echo
echo "==================== 结果 ===================="
echo "共 $total 个源文件，失败 $failed 项"
if [ $failed -ne 0 ]; then
    echo "有失败项，请按上面的提示修复"
    exit 1
fi
echo "全部编译通过、全部示例通过"
