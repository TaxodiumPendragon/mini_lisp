# mini_lisp

> 2024软件设计实践课程的大作业，使用C/C++实现一个mini_lisp

## `Mini-Lisp` 脚手架

### 准备步骤

- 使用 [VS Code Config Helper v4](https://v4.vscch.tk) 配置一遍。这只是确保你正确安装了编译器。
  - 如果你确定你安装了相关编译器，或者之前配置过，则可以跳过这一步。
  - 请在“选择编译器”中，切换编译器类型为你想要用的类型（MinGW 或 MSVC 或 Apple Clang）。
  - 配置完成后，可**直接删除工作文件夹**，这里不需要它。
- 安装 [Cmake](https://cmake.org/download/#latest)。
  - （Windows 11）你可以使用 `winget` 安装：执行 `winget install cmake`。
  - （macOS）如果你安装了 `brew`，则可以执行 `brew install cmake`。
- 通过 VS Code 打开本文件夹后，呼出命令面板（Ctrl/Cmd + Shift + P），执行 `Tasks: Run task...` 中的 `configure` 任务。
- 安装 CMake Tools 扩展后，重新加载窗口以生成智能提示。如果你安装了 Clangd 及其 VS Code 扩展，则按照 `.vscode/c_cpp_properties.json` 中的注释进行操作。

### 编译、运行与调试

- 按 Ctrl + Shift + B 编译项目。按 F6（即你在 VS Code Config Helper 中选择的运行快捷键）运行项目。
- 按 F5 调试项目。

> 你每次新建文件后，你可能需要重新配置项目——执行上方说明中关于 `configure` 任务的描述。
