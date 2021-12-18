/*
 *   Copyright (C) 2021 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
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

/*
	SqApp
	|
	`--- MyApp
 */

struct MyApp : Sq::AppMethod                  // <-- 1. inherit C++ member function(method)
{
	SQ_APP_MEMBERS;                           // <-- 2. inherit member variable
/*	// ------ SqApp members ------
	Sqdb                *db;
	SqdbConfig          *db_config;
	const char          *db_database;
	const SqMigration  **migrations;
	int                  n_migrations;
	SqStorage           *storage;
 */

	// ------ MyApp members ------            // <-- 3. Add variable and non-virtual function in derived struct.

	// user data
	MyApp() {
	}
	~MyApp() {
	}
};


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
