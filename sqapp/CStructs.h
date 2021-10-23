#ifndef C_STRUCTS_H
#define C_STRUCTS_H

#include <time.h>      // time_t
#include <stdint.h>    // int64_t

// C structures for SQL tables


/* define structure User, SQL table "users"
   This structure is used by ../database/migrations/2021_10_12_000000_create_users_table.c
 */
typedef struct User {
	unsigned int   id;
	char          *name;
	char          *email;
	time_t         created_at;
	time_t         updated_at;
} User;


#endif  // C_STRUCTS_H
