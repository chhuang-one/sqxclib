#include <SqStorage.h>
#include <SqMigration.h>
#include <CStructs.h>
#include "migrations-files"

const SqMigration *migrations[] = {
    NULL,                              // migration not existed or deleted

    #include "migrations-elements"
};

const int  n_migrations = sizeof(migrations) / sizeof(void*);
