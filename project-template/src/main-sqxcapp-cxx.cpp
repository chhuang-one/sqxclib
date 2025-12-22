/* main-sqxcapp-cxx.cpp
 * This program link 'sqxc' and 'sqxcapp' libraries.
 * It will link C++ library 'sqxcpp' if you use SQ_TYPE_STD_STRING (SqType for std::string) or others.
 */

#include <sqxc/sqxclib.h>
#include <sqxc/app/SqApp.h>

int  main(void)
{
	Sq::App *myapp = new Sq::App;

#if 0
	// open database that defined in SqApp-config.h
	if (myapp->openDatabase(NULL) != SQCODE_OK)
		return EXIT_FAILURE;
#else
	// open user specified database
	if (myapp->openDatabase("myapp-cxx") != SQCODE_OK)
		return EXIT_FAILURE;
#endif

	/*	If you use command-line program "sqtool-cpp" to do migrate,
		you can remove below migrate() code.
	 */
	// if the database vesion is 0 (no migrations have been done)
	if (myapp->makeSchema() == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (myapp->migrate() != SQCODE_OK)
			return EXIT_FAILURE;
	}

	// Your code can start here...
//	SqStorage *storage = myapp->storage;

	return EXIT_SUCCESS;
}
