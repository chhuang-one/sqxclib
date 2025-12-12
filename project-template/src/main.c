/* main.c
 * This program link 'sqxc' library.
 */

#include <stdio.h>
#include <sqxclib.h>

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

int  main(void)
{
	Sqdb       *db;
	SqStorage  *storage;

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE

	SqdbConfigSqlite  config_sqlite = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION;      // No migration mode
//		.folder = "/tmp",
		.folder = ".",
		.extension = "db",
	};

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config_sqlite);

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE

	SqdbConfigMysql  config_mysql = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION;      // No migration mode
		.host     = "localhost",
		.port     = 3306,
		.user     = "root",
		.password = "",
	};

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config_mysql);

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE

	SqdbConfigPostgre  config_postgre = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION;      // No migration mode
		.host     = "localhost",
		.port     = 5432,
		.user     = "postgres",
		.password = "",
	};

	db = sqdb_new(SQDB_INFO_POSTGRE, (SqdbConfig*) &config_postgre);

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
