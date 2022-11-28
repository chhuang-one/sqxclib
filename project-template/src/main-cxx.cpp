/* main-cxx.cpp
 * This program link 'sqxc' library.
 * It will link C++ library 'sqxcpp' if you use SQ_TYPE_STD_STRING (SqType for std::string) or others.
 */

#include <iostream>
#include <sqxclib.h>

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

int  main(void)
{
	Sq::DbMethod *db;
	Sq::Storage  *storage;

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE

	SqdbConfigSqlite  config_sqlite;

	config_sqlite.folder    = ".";    // "/tmp"
	config_sqlite.extension = "db";

	db = new Sq::DbSqlite(&config_sqlite);

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE

	SqdbConfigMysql  config_mysql;

	config_mysql.host = "localhost";
	config_mysql.port = 3306;
	config_mysql.user = "root";
	config_mysql.password = "";

	db = new Sq::DbMysql(&config_mysql);

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE

	SqdbConfigPostgre  config_postgre;

	config_postgre.host = "localhost";
	config_postgre.port = 5432;
	config_postgre.user = "postgres";
	config_postgre.password = "";

	db = new Sq::DbPostgre(&config_postgre);

#else
	db = NULL;
#endif

	if (db == NULL) {
		std::cerr << "No supported database" << std::endl;
		return EXIT_FAILURE;
	}

	// Your code can start here...
	storage = new Sq::Storage(db);

	if (storage->open("main-cxx") != SQCODE_OK) {
		std::cerr << "Can't open database - " << "main-cxx" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
