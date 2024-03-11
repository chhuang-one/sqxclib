/*
 *   Copyright (C) 2021-2024 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>        // snprintf()

#include <SqError.h>
#include <SqType.h>
#include <SqxcValue.h>
#include <SqMigration.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

/* use C99 designated initializer to
   define constant pointer array of SqColumn that used by constant SqType
 */
static const SqColumn *column_ptrs[3] = {
	&(SqColumn) {SQ_TYPE_INT,    "batch",     offsetof(SqMigrationTable, batch),
	             0,                                              .size = 16},
	&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(SqMigrationTable, id),
	             SQB_COLUMN_PRIMARY | SQB_COLUMN_AUTOINCREMENT,  .size = 16},
	&(SqColumn) {SQ_TYPE_STR,    "migration", offsetof(SqMigrationTable, migration),
	             0,                                              .size = 255},
};

static void sq_migration_table_free(SqMigrationTable *mtable)
{
	free(mtable->migration);
	free(mtable);
}

/* column_ptrs are sorted by name, specify SQB_TYPE_SORTED. */
const SqType SqType_migration_table_ = SQ_TYPE_INITIALIZER_FULL(SqMigrationTable,
                                                                column_ptrs, SQB_TYPE_SORTED,
                                                                NULL, sq_migration_table_free);

// ----------------------------------------------------------------------------
// SqMigrationTable functions

int  sq_migration_install(Sqdb *db)
{
	SqBuffer  buffer = {0};
	SqTable  *table;
	int       code;

	table = sq_table_new(SQ_MIGRATION_TABLE_NAME, &SqType_migration_table_);
	sqdb_sql_create_table(db, &buffer, table, NULL, true);
	code = sqdb_exec(db, buffer.mem, NULL, NULL);
	sq_buffer_final(&buffer);
	sq_table_free(table);
	return code;
}

int  sq_migration_get_last(SqStorage *storage, int *batch)
{
	SqMigrationTable  mtable = {-1, NULL, 0};

	sqxc_value_container(storage->xc_input) = NULL;
	sqxc_value_element(storage->xc_input)   = &SqType_migration_table_;
	sqxc_value_instance(storage->xc_input)  = &mtable;

	sqdb_exec(storage->db,
	          "SELECT id, batch FROM migrations WHERE id=(SELECT MAX(id) FROM migrations)",
	          storage->xc_input, NULL);

	if (batch)
		*batch = mtable.batch;
	return mtable.id;
}

int  sq_migration_count_batch(SqStorage *storage, int batch)
{
	SqBuffer  *buf;
	SqxcValue *xc_value;

	xc_value = (SqxcValue*)storage->xc_input;
	buf = sqxc_get_buffer(xc_value);
	buf->writed = 0;
	sq_buffer_write(buf, "SELECT COUNT(batch) FROM migrations WHERE batch=");
	sprintf(sq_buffer_alloc(buf, snprintf(NULL, 0, "%d", batch)), "%d", batch);

	// configure SqxcValue
	xc_value->container = NULL;
	xc_value->element   = SQ_TYPE_INT;
	xc_value->instance  = &xc_value->value.integer;
	// execute SQL statement and get result
	xc_value->value.integer = 0;
	sqdb_exec(storage->db, buf->mem, (Sqxc*)xc_value, NULL);
	return xc_value->value.integer;
}

int  sq_migration_insert(SqStorage *storage, SqMigration **migrations, int index, int n, int batch)
{
	SqMigrationTable mtable;
	int  code = SQCODE_OK;

	// begin transaction to improve SQLite performance
	if (storage->db->info->product == SQDB_PRODUCT_SQLITE && n > 1)
		code = sq_storage_begin_trans(storage);

	for (int end = index + n;  index < end;  index++) {
		mtable.id = index;
		mtable.migration = (char*)migrations[index]->name;
		mtable.batch = batch;
		if (mtable.migration == NULL)
			mtable.migration = "";
		sq_storage_insert(storage, SQ_MIGRATION_TABLE_NAME,
		                  &SqType_migration_table_, &mtable);
	}

	// commit transaction to improve SQLite performance
	if (storage->db->info->product == SQDB_PRODUCT_SQLITE && n > 1)
		code = sq_storage_commit_trans(storage);    // SQLite performance

	return code;
}

void  sq_migration_remove(SqStorage *storage, int begin)
{
	SqBuffer  buffer = {0};

	sq_buffer_alloc(&buffer, snprintf(NULL, 0, "WHERE id >= %d", begin));
	snprintf(buffer.mem, buffer.size, "WHERE id >= %d", begin);
	sq_storage_remove_all(storage, SQ_MIGRATION_TABLE_NAME, buffer.mem);
	sq_buffer_final(&buffer);
}
