/* main-sqxcapp.c
 * This program link 'sqxc' and 'sqxcapp' libraries.
 */

#include <sqxclib.h>
#include <SqApp.h>

int  main(void)
{
	SqApp *myapp;

	myapp = sq_app_new(SQ_APP_DEFAULT);

#if 0
	// open database that defined in SqApp-config.h
	if (sq_app_open_database(myapp, NULL) != SQCODE_OK)
		return EXIT_FAILURE;
#else
	// open user specified database
	if (sq_app_open_database(myapp, "myapp-db") != SQCODE_OK)
		return EXIT_FAILURE;
#endif

	/*	If you use command-line program "sqxctool" to do migrate,
		you can remove below sq_app_migrate() code.
	 */
	// if the database vesion is 0 (no migrations have been done)
	if (sq_app_make_schema(myapp, 0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(myapp, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	// Your code can start here...
//	SqStorage *storage = myapp->storage;

	return EXIT_SUCCESS;
}
