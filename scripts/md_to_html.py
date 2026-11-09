#!/usr/bin/env python3
# ============================================================
# 把 docs/cpp-interview-guide.md 渲染成可打印的 HTML，供 Chrome 导出 PDF。
#
# 为什么需要它：Chrome 直接打开 .md 不会渲染（只会显示纯文本）或样式很丑，
# 所以先用 Python 的 markdown 库把 md 转成带样式的 HTML，再用
#   google-chrome --headless --print-to-pdf
# 导出成 docs/cpp-interview-guide.pdf。
#
# 用法（在仓库根目录）：
#   pip install markdown
#   python3 scripts/md_to_html.py docs/cpp-interview-guide.md docs/cpp-interview-guide.html
#   google-chrome --headless --disable-gpu --no-sandbox \
#     --print-to-pdf=docs/cpp-interview-guide.pdf docs/cpp-interview-guide.html
#
# 只需要在修改了 md 内容后重新生成 PDF 时才用到这个脚本。
# ============================================================

import re
import sys

try:
    import markdown
except ImportError:  # pragma: no cover
    sys.exit("缺少依赖：请先执行 pip install markdown")

STYLE = """
<style>
  body {
    font-family: -apple-system, "PingFang SC", "Microsoft YaHei", "Noto Sans CJK SC", sans-serif;
    max-width: 880px;
    margin: 32px auto;
    padding: 0 24px;
    line-height: 1.7;
    color: #1f2328;
    font-size: 15px;
  }
  h1 { font-size: 28px; border-bottom: 2px solid #d0d7de; padding-bottom: 8px; }
  h2 { font-size: 22px; margin-top: 32px; border-bottom: 1px solid #d0d7de; padding-bottom: 6px; }
  h3 { font-size: 18px; margin-top: 24px; }
  code {
    font-family: "SFMono-Regular", Consolas, "Liberation Mono", monospace;
    background: #f6f8fa;
    padding: 2px 5px;
    border-radius: 4px;
    font-size: 13px;
  }
  pre {
    background: #f6f8fa;
    border: 1px solid #d0d7de;
    border-radius: 6px;
    padding: 12px 14px;
    overflow-x: auto;
    page-break-inside: avoid;
  }
  pre code { background: none; padding: 0; font-size: 12.5px; line-height: 1.5; }
  table { border-collapse: collapse; width: 100%; margin: 12px 0; font-size: 14px; }
  th, td { border: 1px solid #d0d7de; padding: 6px 10px; text-align: left; }
  th { background: #f6f8fa; }
  blockquote {
    margin: 12px 0;
    padding: 4px 14px;
    color: #57606a;
    border-left: 4px solid #d0d7de;
  }
  @page { margin: 16mm 14mm; }
</style>
"""


# PDF 里的相对链接会变成 file:///src/... 这种打不开的地址，
# 所以导出前把 ../ 开头的相对路径换成 GitHub 上的绝对地址。
REPO_URL = "https://github.com/Controlbrother/agent_test/blob/main"


def absolutize_links(html: str) -> str:
    def replace_relative(match: "re.Match[str]") -> str:
        target = match.group(2)
        target = target[3:] if target.startswith("../") else target
        return f'{match.group(1)}="{REPO_URL}/{target}"'

    return re.sub(r'(href)="(\.\./[^"]+|src/[^"]+)"', replace_relative, html)


def main() -> None:
    if len(sys.argv) != 3:
        sys.exit("用法: python3 scripts/md_to_html.py <输入.md> <输出.html>")

    input_path, output_path = sys.argv[1], sys.argv[2]
    with open(input_path, encoding="utf-8") as handle:
        text = handle.read()

    html_body = markdown.markdown(
        text,
        extensions=["tables", "fenced_code", "toc", "sane_lists"],
    )
    html_body = absolutize_links(html_body)

    html = (
        "<!DOCTYPE html>\n"
        '<html lang="zh-CN">\n<head>\n<meta charset="utf-8">\n'
        "<title>C++ 面试刷题知识库</title>\n"
        f"{STYLE}\n</head>\n<body>\n{html_body}\n</body>\n</html>\n"
    )

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write(html)
    print(f"已生成 {output_path}")


if __name__ == "__main__":
    main()
