#ifndef C_STRUCTS_H
#define C_STRUCTS_H

#include <time.h>         // time_t
#include <stdint.h>       // int64_t

// C/C++ structures for database tables

/* To create "users" table in database schema:

   1. Define structure 'User' for database table "users" in this header file.

   2. Run the following command in the console to generate C migration file:

      $ sqtool  make:migration  create_users_table

      This will append data in migrations-declarations
                               migrations-elements
                               migrations-files.c

   3. Edit generated migration file is in ../database/migrations/yyyy_MM_dd_HHmmss_create_users_table.c
 */
typedef struct User {
	int        id;
	int        age;
	char      *name;
	char      *email;
	time_t     created_at;
	time_t     updated_at;
} User;


#endif  // C_STRUCTS_H
