/*
   include C source files in ../database/migrations
   if migration not existed or deleted, erase line that has file path.
 */

#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"
#include "migrations.h"


/* C source files in ../database/migrations */

#include "../database/migrations/2021_10_12_000000_create_users_table.c"
