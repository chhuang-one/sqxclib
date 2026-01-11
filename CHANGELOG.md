[中文](CHANGELOG.cn.md)

1.1.1 (Jan 11, 2026)
=====
This release add "No migration mode", column/table comments, a new JSON converter using [cJSON](https://github.com/DaveGamble/cJSON), and
change directories and file names to avoid problem in future. It also fix multiple memory leak and bugs.

Features:
---------
* A new JSON converter using cJSON is provided, while the old json-c converter can still be used.
  When you enable SQ_CONFIG_SQXC_CJSON_ONLY_IF_POSSIBLE in SqConfig.h and cJSON is available,
  SqxcJsonc (using json-c) will not be compiled and SqxcCjson (using cJSON) will be default JSON converter.

* SqxcJson is name of default JSON converter. It's alias for SqxcCjson or SqxcJsonc.

* No migration mode. If developer doesn't need to sync migrations to database.
  To enable no migration mode. Please set SQDB_CONFIG_NO_MIGRATION in SqdbConfig::bit_field.
  SqdbConfig::bit_field = SQDB_CONFIG_NO_MIGRATION;

* MySQL & PostgreSQL support adding comments to tables and columns.

* Add CMake module FindSqxc.cmake, the project-template also contains this.

Other Changes:
--------------
* The sqxclib.h file now includes sqxcapp.h and sqxcsupport.h,
  so you no longer need to include them separately when using SqApp and SqRow.

* Change relative path of header files in source files.
  It is recommended to change include path from <sqxclib.h> to <sqxc/sqxclib.h>.

* Merge sqxcapp-user and sqxcapptool-user into sqapp-data.

* Move directory 'sqxcsupport' to 'sqxc/support', part of 'sqxcapp' move to 'sqxc/app'.

* Rename directory 'sqxcapp' to 'sqapp'.

* Rename C++ source file 'sqapp/sqtool-cpp.cpp' to 'sqapp/sqtool-cxx.cpp'.

* Rename C++ migrations file 'sqapp/migrations-files.cpp' to 'sqapp/migrations-files-cxx.cpp'.

* If you use project-template to create project, please do these in your project directory:
  1. Backup your project and install/upgrade sqxclib 1.1.1 to system.
  2. Rename directory from 'sqxcapp' to 'sqapp'.
  3. Rename sqapp/migrations-files.cpp (or migrations-files-cpp.cpp) to sqapp/migrations-files-cxx.cpp
  4. Besides migrations*, SqApp-config.*, and CStructs.h files in directory 'sqapp', 
     copy others from directory "project-template" to your project directory.

* Rename C++ header file 'sqxc/SqType-stl-cpp.h' to 'sqxc/SqType-stl-cxx.h'

* Rename C++ library 'sqxcpp' to 'sqxcxx'

Enhancements:
-------------
SqType: allows conversion of int64 to floating-point numbers.
SqType: convert signed and unsigned integer each other.

Fixes:
------
* PostgreSQL should use length(columnName) as "length(columnName)" to get data length.
* PostgreSQL should use numeric type if developer specify precision.
* SQL syntax error occurred when retrieving blob binary from MySQL/MariaDB.
* segmentation fault when calling sq_storage_update_field() in macOS.
* Storage.query() used the wrong type to release the instance.
* Add "inline" to the function definitions in SqRow.h to avoid redefining.
* fix multiple memory leak.
* project template cannot build targets.
  - The library must be reinstalled and update project files from project template.
