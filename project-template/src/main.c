/* main.c
 * This program link 'sqxc' library.
 */

#include <stdio.h>
#include <sqxclib.h>

#define USE_SQLITE_IF_POSSIBLE    1

int  main(void)
{
	Sqdb       *db;
	SqStorage  *storage;

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE

	SqdbConfigSqlite  config_sqlite = {
//		.folder = "/tmp",
		.folder = ".",
		.extension = "db",
	};

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config_sqlite);

#elif SQ_CONFIG_HAVE_MYSQL

	SqdbConfigMysql  config_mysql = {
		.host     = "localhost",
		.port     = 3306,
		.user     = "root",
		.password = "",
	};

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config_mysql);

#else
	db = NULL;
#endif

	if (db == NULL) {
		fprintf(stderr, "No supported database\n");
		return EXIT_FAILURE;
	}

	// Your code can start here...
	storage = sq_storage_new(db);

	if (sq_storage_open(storage, "main") != SQCODE_OK) {
		fprintf(stderr, "Can't open database - %s\n", "main");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
