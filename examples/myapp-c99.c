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

#include <sqxclib.h>
#include <SqApp.h>

typedef struct MyApp    MyApp;

struct MyApp
{
	SqApp  base;

	// user data
};

MyApp *my_app_new()
{
	MyApp *myapp;

	myapp = malloc(sizeof(MyApp));
	sq_app_init(&myapp->base);

	return myapp;
}

void  my_app_free(MyApp *myapp)
{
	sq_app_final(&myapp->base);
	free(myapp);
}


int  main(void)
{
	MyApp *myapp;

	myapp = my_app_new();

#if 0
	// open database that defined in SqApp-config.h
	if (sq_app_open_database(&myapp->base, NULL) != SQCODE_OK)
		return EXIT_FAILURE;
#else
	// open user specify database
	if (sq_app_open_database(&myapp->base, "myapp-db") != SQCODE_OK)
		return EXIT_FAILURE;
#endif

	// if the database vesion is 0 (no migrations have been done)
	if (sq_app_make_schema(&myapp->base, 0) == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(&myapp->base, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	// run your code here...

	sq_app_close_database(&myapp->base);
	my_app_free(myapp);

	return EXIT_SUCCESS;
}
