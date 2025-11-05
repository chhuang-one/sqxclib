/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

/* This is sample code for sqxclib and sqxcapp */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>       // cout
#include <type_traits>    // is_standard_layout<>

#include <sqxclib.h>
#include <SqApp.h>
#include <CStructs.h>     // struct User

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

	Sq::Storage *getStorage() {
		return (Sq::Storage*)base.storage;
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

	Sq::Storage *getStorage() {
		return (Sq::Storage*)storage;
	}
};
#endif  // USE_DERVIDED_MYAPP

// In database table "users", do Create, Read, Update, Delete.
int  usersCrud(Sq::Storage *storage)
{
	Sq::Table *userTable;
	Sq::Type  *userType;
	User     user = {0};
	User    *userPtr;
	int64_t  id[2];
	int64_t  n;

	/*	If database table "users" is defined in C language but application is written in C++ language,
		You must change C language type names to C++ type names because
		template functions of Sq::Storage use type name to get Sq::Table.

		See the documentation  doc/database-migrations.md  for more information on this.

		If your database table "users" is defined in C++, you can remove following code.
	 */
	userTable = storage->schema->find("users");
	if (userTable == NULL)
		return EXIT_FAILURE;
	// If userType.name is defined in C language, change type names to C++ type names.
	userType = (Sq::Type*)userTable->type;
	userType->setName(typeid(User).name());


	/* CRUD --- insert, update, get, remove */
	user.age   = 18;
	user.email = (char*)"alex@guest";
	user.name  = (char*)"Alex";
	id[0] = storage->insert("users", &user);
	user.age   = 28;
	user.name  = (char*)"Alan";
	user.email = (char*)"alan@guest";
	id[1] = storage->insert("users", &user);

	// update columns - "age" and "name"
	user.age   = 21;
	user.name  = (char*)"Ti";
	n = storage->updateAll("users", &user,
	                       "WHERE id > 0",
	                       "age", "name");
	std::cout << "number of rows changed : " << n << std::endl;

	userPtr = (User*)storage->get("users", id[0]);
	if (userPtr) {
		std::cout << std::endl
		          << "User::age   = " << userPtr->age   << std::endl
		          << "User::name  = " << userPtr->name  << std::endl
		          << "User::email = " << userPtr->email << std::endl;
		// free "User" instance
		userType->finalInstance(userPtr);
		free(userPtr);
	}

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
	// update fields - User::email and User::name
	user.age   = 38;
	user.name  = (char*)"Sky";
	user.email = (char*)"sky@host";
	n = storage->updateField("users", &user,
	                         "WHERE id > 0",
	                         &User::name,
	                         &User::email);
	std::cout << "number of rows changed : " << n << std::endl;
#endif // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

	userPtr = (User*)storage->get("users", id[1]);
	if (userPtr) {
		std::cout << std::endl
		          << "User::age   = " << userPtr->age   << std::endl
		          << "User::name  = " << userPtr->name  << std::endl
		          << "User::email = " << userPtr->email << std::endl;
		// free "User" instance
		userType->finalInstance(userPtr);
		free(userPtr);
	}

	storage->removeAll("users");
	return EXIT_SUCCESS;
}

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

	/*	If you use command-line program "sqtool-cpp" to do migrate,
		you can remove below 'myapp->migrate()' code.
	 */
	// if the version of schema in database is 0 (no migrations have been done)
	if (myapp->makeSchema() == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (myapp->migrate() != SQCODE_OK)
			return EXIT_FAILURE;
	}


	/*	strcut User is defined in  sqxcapp/CStructs.h
		Database table "users" is defined in  database/migrations/2021_10_12_000000_create_users_table.cpp
		but it is disabled by default.

		To enable the table "users", modify following files:
			migrations-declarations
			migrations-elements
			migrations-files.cpp
	 */
	Sq::Storage *storage = myapp->getStorage();

	// In database table "users", do Create, Read, Update, Delete.
	usersCrud(storage);

	myapp->closeDatabase();
	delete myapp;

	return EXIT_SUCCESS;
}
