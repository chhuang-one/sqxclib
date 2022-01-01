/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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
#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqxcValue.h>
#include <SqMigration.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

/* use C99 designated initializer to
   define constant SqColumn pointer array that used by constant SqType
 */
static const SqColumn *column_ptrs[3] = {
	&(SqColumn) {SQ_TYPE_INT,    "batch",     offsetof(SqMigrationTable, batch),     0,
	             .size = 16},
	&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(SqMigrationTable, id),        SQB_PRIMARY | SQB_INCREMENT,
	             .size = 16},
	&(SqColumn) {SQ_TYPE_STRING, "migration", offsetof(SqMigrationTable, migration), 0,
	             .size = 255},
};

static void sq_migration_table_free(SqMigrationTable *mtable)
{
	free(mtable->migration);
	free(mtable);
}

/* column_ptrs are sorted by name, pass SQB_TYPE_SORTED to last argument. */
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
	SqBuffer *buf;

	buf = sqxc_get_buffer(storage->xc_input);
	buf->writed = 0;
	sq_buffer_write(buf, "SELECT COUNT(batch) FROM migrations WHERE batch=");
	sprintf(sq_buffer_alloc(buf, snprintf(NULL, 0, "%d", batch)), "%d", batch);

	storage->xc_input->value.integer = 0;
#if 1
	sqxc_value_container(storage->xc_input) = NULL;
	sqxc_value_element(storage->xc_input)   = SQ_TYPE_INT;
	sqxc_value_instance(storage->xc_input)  = &storage->xc_input->value;
#else
	sqxc_ctrl(storage->xc_input, SQXC_VALUE_CTRL_BUILTIN, SQ_TYPE_INT);
#endif
	sqdb_exec(storage->db, buf->mem, storage->xc_input, NULL);
	return storage->xc_input->value.integer;
}

int  sq_migration_insert(SqStorage *storage, SqMigration **migrations, int index, int n, int batch)
{
	SqMigrationTable mtable;
	int  code;

	// begin transaction to improve SQLite performance
	if (storage->db->info->product == SQDB_PRODUCT_SQLITE && n > 1)
		code = sq_storage_begin_trans(storage);

	for (int end = index + n;  index < end;  index++) {
		mtable.id = index;
		mtable.migration = (char*)migrations[index]->name;
		mtable.batch = batch;
		if (mtable.migration == NULL)
			mtable.migration = "";
		sq_storage_insert_full(storage, SQ_MIGRATION_TABLE_NAME, NULL,
		                       &mtable, &SqType_migration_table_);
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
	sq_storage_remove_by_sql(storage, SQ_MIGRATION_TABLE_NAME, buffer.mem);
	sq_buffer_final(&buffer);
}
