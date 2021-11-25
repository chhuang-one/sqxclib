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

	// open database that defined in SqApp-config.h
//	sq_app_open_database(&myapp->base, NULL);

	// open user specify database
	sq_app_open_database(&myapp->base, "myapp-db");

	// run migrations that defined in ../database/migrations
	sq_app_migrate(&myapp->base, 0);

	sq_app_close_database(&myapp->base);
	my_app_free(myapp);
}
