/*
 *   Copyright (C) 2021-2022 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <iostream>     // cout
#include <type_traits>  // is_standard_layout<>

#include <sqxclib.h>
#include <SqApp.h>

#define USE_DERVIDED_MYAPP    0

#if USE_DERVIDED_MYAPP == 0
/*	In this case, MyApp can be C++ standard layout or NOT. */
class MyApp
{
private:
	Sq::App  base;

public:
	int   openDatabase(const char *db_database = NULL) {
		return base.openDatabase(db_database);
	}
	void  closeDatabase(void) {
		base.closeDatabase();
	}
	int   makeSchema(int migration_id = 0) {
		return base.makeSchema(migration_id);
	}
	int   migrate(int step = 0) {
		return base.migrate(step);
	}
	int   rollback(int step = 0) {
		return base.rollback(step);
	}
};

#else
/*	In this case, MyApp must be C++ standard layout.

	SqApp
	|
	`--- MyApp
 */
struct MyApp : Sq::AppMethod                  // <-- 1. inherit C++ member function(method)
{
	SQ_APP_MEMBERS;                           // <-- 2. inherit member variable

	// add MyApp members                      // <-- 3. Add variable and non-virtual function in derived struct.

	MyApp() {
		init();     // call Sq::AppMethod::init()
	}
	~MyApp() {
		final();    // call Sq::AppMethod::final()
	}
};
#endif  // USE_DERVIDED_MYAPP

int  main(void)
{
	MyApp *myapp = new MyApp;

	std::cout << "is_standard_layout<MyApp> = "
	          << std::is_standard_layout<MyApp>::value
	          << std::endl << std::endl;

#if 0
	// open database that defined in SqApp-config.h
	if (myapp->openDatabase(NULL) != SQCODE_OK)
		return EXIT_FAILURE;
#else
	// open user specified database
	if (myapp->openDatabase("myapp-cxx") != SQCODE_OK)
		return EXIT_FAILURE;
#endif

	// if the database vesion is 0 (no migrations have been done)
	if (myapp->makeSchema() == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (myapp->migrate() != SQCODE_OK)
			return EXIT_FAILURE;
	}

	// run your code here...

	myapp->closeDatabase();
	delete myapp;

	return EXIT_SUCCESS;
}
