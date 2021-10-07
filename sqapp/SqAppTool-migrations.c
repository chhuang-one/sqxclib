#define SQ_APP_TOOL

// --- below source code is the same as migrations.c
#include <SqMigration.h>
#include "migrations-files"

const SqMigration *migrations[] = {
    NULL,
    #include "migrations-elements"
};

const int  n_migrations = sizeof(migrations) / sizeof(void*);
