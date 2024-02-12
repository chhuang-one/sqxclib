#ifndef C_STRUCTS_H
#define C_STRUCTS_H

#include <time.h>      // time_t
#include <stdint.h>    // int64_t

// C/C++ structures for SQL tables


/* 1. define structure 'User' for SQL table "users".
   2. run console command to generate migration file:
      sqxctool make:migration create_users_table
   3. edit generated migration file is in ../database/migrations/yyyy_MM_dd_HHmmss_create_users_table.c
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
