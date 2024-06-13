[English](README.md)

# sqxclib 的项目模板

该目录包含用于创建 sqxclib 项目的文件和目录。  
在使用这些文件之前，您必须在系统中安装 sqxclib 头文件和库。

## 安装 sqxclib

您可以使用 CMake 或 meson 来构建和安装 sqxclib。  
如果在加载共享库时出错，您可能需要设置 LD_LIBRARY_PATH 环境变量。  
  
例如: sqxclib 共享库安装在 /usr/local/lib

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
export LD_LIBRARY_PATH
```

## 创建项目
1. 创建项目目录。
2. 将此目录中的所有文件复制到项目目录。
3. 使用 CMake 或 meson 构建项目。

## 项目目录

#### /sqxcapp
此目录包含 sqtool 和静态库 'sqxcapp-user'、'sqxcapptool-user'。

#### /database/migrations
此目录包含由 sqtool（或 sqtool-cpp）生成的 C 或 C++ 迁移文件。  
CMake 或 meson 会将这些迁移文件编译成静态库 'sqxcapp-user' 和 'sqxcapptool-user'。

#### /src
该目录包含 4 种可执行目标：

1. ***main.c***  仅使用 'sqxc' 共享库。
2. ***main-cxx.cpp***  与 ***main.c*** 相同，但使用 C++ 语言。
3. ***main-sqxcapp.c***  使用 sqtool 和 'sqxc' 和 'sqxcapp' 共享库。
4. ***main-sqxcapp-cxx.cpp***  与 ***main-sqxcapp.c*** 相同，但使用 C++ 语言。
