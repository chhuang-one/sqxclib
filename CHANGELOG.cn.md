[English](CHANGELOG.md)

1.1.1 (2026年1月11日)
=====
此版本新增 "无迁移模式"、列/表注释，使用 [cJSON](https://github.com/DaveGamble/cJSON) 的新 JSON 转换器，以及
更改目录和文件名以避免将来出现问题。同时还修复了多个内存泄漏和错误。

功能：
------
* 提供了一个使用 cJSON 的全新 JSON 转换器，同时您仍然可以使用旧的 json-c 转换器。
  当您在 SqConfig.h 中启用 SQ_CONFIG_SQXC_CJSON_ONLY_IF_POSSIBLE 并且 cJSON 可用时，
  SqxcJsonc (使用 json-c) 将不会被编译，SqxcCjson (使用 cJSON) 将成为默认的 JSON 转换器。

* SqxcJson 是默认 JSON 转换器的名称。它是 SqxcCjson 或 SqxcJsonc 的别名。

* 无迁移模式。如果开发者不需要将迁移同步到数据库，则可以使用此模式。
  要启用无迁移模式，请在 SqdbConfig::bit_field 中设置 SQDB_CONFIG_NO_MIGRATION。
  SqdbConfig::bit_field = SQDB_CONFIG_NO_MIGRATION;

* MySQL 和 PostgreSQL 支持向表和列添加注释。

* 添加 CMake 模块 FindSqxc.cmake，项目模板中也包含此模块。

其他变更：
----------
* sqxclib.h 文件现在包含了 sqxcapp.h 和 sqxcsupport.h，因此
  在使用 SqApp 和 SqRow 时不再需要单独包含它们。

* 更改源代码文件中头文件的相对路径。
  建议将包含路径从 <sqxclib.h> 更改为 <sqxc/sqxclib.h>。

* 将 sqxcapp-user 和 sqxcapptool-user 合并到 sqapp-data 中。

* 将目录 'sqxcsupport' 移至 'sqxc/support'，将 'sqxcapp' 的一部分移至 'sqxc/app'。

* 将目录 'sqxcapp' 重命名为 'sqapp'。

* 将 C++ 源文件 'sqapp/sqtool-cpp.cpp' 重命名为 'sqapp/sqtool-cxx.cpp'。

* 将 C++ 迁移文件 'sqapp/migrations-files.cpp' 重命名为 'sqapp/migrations-files-cxx.cpp'。

* 如果您使用项目模板创建项目，请在项目目录中执行以下操作：
  1. 备份您的项目并将 sqxclib 1.1.1 安装/升级到系统。
  2. 将目录名从 'sqxcapp' 重命名为 'sqapp'。
  3. 将 sqapp/migrations-files.cpp（或 migrations-files-cpp.cpp）重命名为 sqapp/migrations-files-cxx.cpp。
  4. 除了目录 'sqapp' 中的 migrations*、SqApp-config.* 和 CStructs.h 文件之外，
     请将目录 'project-template' 中的其他文件复制到您的项目目录中。

* 将 C++ 头文件 'sqxc/SqType-stl-cpp.h' 重命名为 'sqxc/SqType-stl-cxx.h'

* 将 C++ 库 'sqxcpp' 重命名为 'sqxcxx'

增强功能：
----------
* SqType：允许将 int64 转换为浮点数。
* SqType：支持有符号整数和无符号整数之间的相互转换。

修复：
------
* PostgreSQL 应使用 length(columnName) as "length(columnName)" 来获取数据长度。
* 如果开发者指定了精度，PostgreSQL 应使用 numeric 类型。
* 从 MySQL/MariaDB 检索 blob 二进制文件时出现 SQL 语法错误。
* 在 macOS 中调用 sq_storage_update_field() 时出现段错误。
* Storage.query() 使用了错误的类型释放实例。
* 在 SqRow.h 的函数定义中添加 "inline" 以避免重新定义。
* 修复多个内存泄漏。
* 项目模板无法构建目标。
  - 必须重新安装库并从项目模板更新项目文件。
