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

#include <SqApp.h>

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

	// open database that defined in SqApp-config.h
//	myapp->openDatabase(NULL);

	// open user specify database
	myapp->openDatabase("myapp-cxx");

	// run migrations that defined in ../database/migrations
	myapp->migrate();

	myapp->closeDatabase();
	delete myapp;
}
