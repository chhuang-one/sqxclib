#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include <SqMigration.h>

// How to mix C and C++ - https://isocpp.org/wiki/faq/mixing-c-and-cpp
#ifdef __cplusplus
extern "C" {
#endif

extern const SqMigration *migrations[];
extern const int          n_migrations;

#include "migrations-declarations"

#ifdef __cplusplus
}
#endif

#endif  // MIGRATIONS_H
