/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#include <SqConfig.h>
#include <SqError.h>
#include <SqType.h>
#include <Sqdb.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

#define SQL_STRING_LENGTH_DEFAULT    SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT

Sqdb   *sqdb_new(const SqdbInfo *info, const SqdbConfig *config)
{
	Sqdb *db;

	db = malloc(info->size);
	sqdb_init(db, info, config);
	return db;
}

void    sqdb_free(Sqdb *db)
{
	sqdb_final(db);
	free(db);
}

void  sqdb_init(Sqdb *db, const SqdbInfo *info, const SqdbConfig *config)
{
	void (*init)(Sqdb *db, const SqdbConfig *conf);

	init = info->init;
	if (init) {
		db->info = info;
		info->init(db, config);
	}
	else {
		memset(db, 0, info->size);
		db->info = info;
	}
}

void  sqdb_final(Sqdb *db)
{
	void (*final)(Sqdb *db);

	final = db->info->final;
	if (final)
		final(db);
}

// ----------------------------------------------------------------------------
// execute SQL statement

int  sqdb_exec_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns)
{
	SqPtrArray  indexes;
	int  rc;

	if (arranged_columns == NULL)
		arranged_columns = sq_type_get_ptr_array(table->type);

	sq_ptr_array_init(&indexes, 4, NULL);
	sq_table_get_columns(table, &indexes, SQ_TYPE_INDEX, 0);    // get record of "CREATE INDEX"
	for (int i = 0;  i < indexes.length;  i++) {
		sqdb_sql_create_index(db, sql_buf, table, (SqColumn*)indexes.data[i]);
		sql_buf->writed = 0;
#ifndef NDEBUG
		// Don't run this because sqdb_exec() will output this debug message.
//		fprintf(stderr, "SQL: %s\n", sql_buf->mem);
#endif
		rc = sqdb_exec(db, sql_buf->mem, NULL, NULL);
		if (rc != SQCODE_OK)
			break;
	}
	sq_ptr_array_final(&indexes);
	return rc;
}

int  sqdb_exec_alter_table(Sqdb *db, SqBuffer *buffer, SqTable *table, SqPtrArray *arranged_columns, SqTable *table_data)
{
	SqColumn *column, *column_data = NULL;
	int       index;
	int       rc;

	if (arranged_columns == NULL)
		arranged_columns = sq_type_get_ptr_array(table->type);
	// ALTER TABLE
	for (index = 0;  index < arranged_columns->length;  index++) {
		column = (SqColumn*)arranged_columns->data[index];
		if (column->bit_field & SQB_COLUMN_CHANGED) {
			// ALTER COLUMN
			sqdb_sql_alter_column(db, buffer, table, column);
		}
		else if (column->name == NULL) {
			// DROP COLUMN / CONSTRAINT / INDEX / KEY
			sqdb_sql_drop_column(db, buffer, table, column);
		}
		else if (column->old_name && (column->bit_field & SQB_COLUMN_RENAMED) == 0) {
			// RENAME COLUMN
			if (table_data) {
				// MySQL "CHANGE COLUMN" need original column data
				column_data = (SqColumn*)sq_ptr_array_find_sorted(sq_type_get_ptr_array(table_data->type), column->old_name,
				                         (SqCompareFunc)sq_entry_cmp_str__name, NULL);
				if (column_data)
					column_data = *(SqColumn**)column_data;
			}
			sqdb_sql_rename_column(db, buffer, table, column, column_data);
		}
		else {
			// ADD COLUMN / CONSTRAINT / INDEX / KEY
			sqdb_sql_add_column(db, buffer, table, column);
		}

		buffer->writed = 0;
#ifndef NDEBUG
		// Don't run this because sqdb_exec() will output this debug message.
//		fprintf(stderr, "SQL: %s\n", buffer->mem);
#endif
		rc = sqdb_exec(db, buffer->mem, NULL, NULL);
		if (rc != SQCODE_OK)
			break;
	}
	return rc;
}

// ----------------------------------------------------------------------------
// write SQL statement to 'sql_buf'

int  sqdb_sql_create_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns, bool primary_first)
{
	int  n_columns;

	if (arranged_columns == NULL)
		arranged_columns = sq_type_get_ptr_array(table->type);

	// CREATE TABLE
	sq_buffer_write(sql_buf, "CREATE TABLE");

	// write "table name"
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ' ';
	sq_buffer_r_at(sql_buf, 0) = db->info->quote.identifier[0];
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(sql_buf, 0) = ' ';

	n_columns = sqdb_sql_create_table_params(db, sql_buf, arranged_columns, primary_first);

	sql_buf->mem[sql_buf->writed] = 0;    // NULL-termainated is not counted in length
	return n_columns;
}

// used by sqdb_sql_create_table_params()
static int  column_cmp_pfk(SqColumn **col1, SqColumn **col2) {
	int  val1, val2;

	if ((*col1)->bit_field & SQB_COLUMN_PRIMARY)
		val1 = 1;
	else if ((*col1)->foreign)
		val1 = 2;
	else
		val1 = 3;

	if ((*col2)->bit_field & SQB_COLUMN_PRIMARY)
		val2 = 1;
	else if ((*col2)->foreign)
		val2 = 2;
	else
		val2 = 3;

	return val1 - val2;
}

int  sqdb_sql_create_table_params(Sqdb *db, SqBuffer *buffer, SqPtrArray *arranged_columns, bool primary_first)
{
	SqPtrArray array;
	SqColumn *column;
	int       index, n_columns = 0;
	bool      has_constraint = false;

	if (primary_first) {
		// if first column is not primary key
		column = (SqColumn*)arranged_columns->data[0];
		if ((column->bit_field & SQB_COLUMN_PRIMARY) == 0) {
			// move primary and foreign key to beginning of array
			sq_ptr_array_init(&array, arranged_columns->length, NULL);
			memcpy(array.data, arranged_columns->data, sizeof(void*) * arranged_columns->length);
			array.length = arranged_columns->length;
			sq_ptr_array_sort(&array, (SqCompareFunc)column_cmp_pfk);
			arranged_columns = &array;
		}
	}

	sq_buffer_write(buffer, "( ");
	for (index = 0;  index < arranged_columns->length;  index++) {
		column = (SqColumn*)arranged_columns->data[index];
		if (column == NULL)
			continue;
		// skip INDEX
		if (column->type == SQ_TYPE_INDEX)
			continue;
		// skip CONSTRAINT
		if (column->type == SQ_TYPE_CONSTRAINT) {
			has_constraint = true;
			continue;
		}

		// write comma between two columns
		if (n_columns > 0) {
			sq_buffer_alloc(buffer, 2);
			buffer->mem[buffer->writed -2] = ',';
			buffer->mem[buffer->writed -1] = ' ';
		}
		n_columns++;
		// write column
		sqdb_sql_write_column(db, buffer, column, NULL);
	}

//	if (db->info->product == SQDB_PRODUCT_MYSQL) {
		for (index = 0;  index < arranged_columns->length;  index++) {
			column = (SqColumn*)arranged_columns->data[index];
			// Don't output CONSTRAINT and INDEX here
			if (column->type == SQ_TYPE_CONSTRAINT || column->type == SQ_TYPE_INDEX)
				continue;
			// FOREIGN KEY
			if (column->foreign)    // || column->bit_field & SQB_COLUMN_FOREIGN
				sq_buffer_write(buffer, ", FOREIGN KEY");
			// PRIMARY KEY
			else if (column->bit_field & SQB_COLUMN_PRIMARY && db->info->product != SQDB_PRODUCT_SQLITE)
				sq_buffer_write(buffer, ", PRIMARY KEY");
			// UNIQUE
			else if (column->bit_field & SQB_COLUMN_UNIQUE)
				sq_buffer_write(buffer, ", UNIQUE");
			else
				continue;

			// write ("column_name")
			sq_buffer_alloc(buffer, 3);
			sq_buffer_r_at(buffer, 2) = ' ';
			sq_buffer_r_at(buffer, 1) = '(';
			sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
			sq_buffer_write(buffer, column->name);
			sq_buffer_alloc(buffer, 2);
			sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
			sq_buffer_r_at(buffer, 0) = ')';

			// FOREIGN KEY REFERENCES
			if (column->foreign)
				sqdb_sql_write_foreign_ref(db, buffer, column);
		}
//	}

	if (has_constraint) {
		for (index = 0;  index < arranged_columns->length;  index++) {
			column = (SqColumn*)arranged_columns->data[index];
			// DROP COLUMN
			if (column->name == NULL)
				continue;
			// CREATE INDEX
			if (column->type == SQ_TYPE_INDEX)
				continue;
			// CONSTRAINT
			if (column->type == SQ_TYPE_CONSTRAINT) {
				sq_buffer_write_c(buffer, ',');
				sqdb_sql_write_constraint(db, buffer, column);
			}
		}
	}

	if (arranged_columns == &array)     //  primary_first
		sq_ptr_array_final(&array);

	sq_buffer_write_c(buffer, ')');
	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
	return n_columns;  
}

void sqdb_sql_drop_table(Sqdb *db, SqBuffer *buffer, SqTable *table, bool if_exist)
{
	// DROP TABLE IF EXISTS "table_name"
	// DROP TABLE "table_name"
	sq_buffer_write(buffer, "DROP TABLE");
	if (if_exist)
		sq_buffer_write(buffer, " IF EXISTS");

	// write "table old name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, table->old_name);
	sq_buffer_write_c(buffer, db->info->quote.identifier[1]);

	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

void sqdb_sql_from(Sqdb *db, SqBuffer *sql_buf, const char *table_name, bool is_delete)
{
	sq_buffer_write(sql_buf, (is_delete) ? "DELETE" : "SELECT");
	if (is_delete == false)
		sq_buffer_write(sql_buf, " *");
	sq_buffer_write(sql_buf, " FROM");

	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ' ';
	sq_buffer_r_at(sql_buf, 0) = db->info->quote.identifier[0];
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(sql_buf, 0) = ' ';

	sql_buf->mem[sql_buf->writed] = 0;    // NULL-termainated is not counted in length
}

void sqdb_sql_rename_table(Sqdb *db, SqBuffer *buffer, const char *old_name, const char *new_name)
{
	// RENAME TABLE "old_name" TO "new_name";
	// ALTER TABLE "old_name" RENAME TO "new_name";
	if (db->info->product == SQDB_PRODUCT_MYSQL)
		sq_buffer_write(buffer, "RENAME TABLE");
	else
		sq_buffer_write(buffer, "ALTER TABLE");

	// write "table old name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, old_name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	if (db->info->product == SQDB_PRODUCT_MYSQL)
		sq_buffer_write(buffer, "TO");
	else
		sq_buffer_write(buffer, "RENAME TO");

	// write "table new name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, new_name);
	sq_buffer_write_c(buffer, db->info->quote.identifier[1]);

	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

// ------------------------------------
// column

void sqdb_sql_add_column(Sqdb *db, SqBuffer *buffer, SqTable *table, SqColumn *column)
{
	// CREATE INDEX
	if (column->type == SQ_TYPE_INDEX) {
		sqdb_sql_create_index(db, buffer, table, column);
		return;
	}

	sq_buffer_write(buffer, "ALTER TABLE");

	// write "table name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, table->name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	sq_buffer_write(buffer, "ADD ");

	// ADD CONSTRAINT
	if (column->type == SQ_TYPE_CONSTRAINT) {
		sqdb_sql_write_constraint(db, buffer, column);
		buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
		return;
	}
	// ADD FOREIGN KEY
	else if (column->foreign) {
		sq_buffer_write(buffer, "FOREIGN KEY");
	}
	// ADD PRIMARY KEY
	else if (column->bit_field & SQB_COLUMN_PRIMARY) {
		sq_buffer_write(buffer, "PRIMARY KEY");
	}
	// ADD UNIQUE
	else if (column->bit_field & SQB_COLUMN_UNIQUE) {
		sq_buffer_write(buffer, "UNIQUE");
	}
	// ADD COLUMN
	else {
		sqdb_sql_write_column(db, buffer, column, NULL);
		buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
		return;
	}

	// ("column_name")
	sq_buffer_alloc(buffer, 3);
	sq_buffer_r_at(buffer, 2) = ' ';
	sq_buffer_r_at(buffer, 1) = '(';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, column->name);
	sq_buffer_alloc(buffer, 3);
	sq_buffer_r_at(buffer, 2) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 1) = ')';
	sq_buffer_r_at(buffer, 0) = ' ';

	// ADD FOREIGN KEY REFERENCES
	if (column->foreign)
		sqdb_sql_write_foreign_ref(db, buffer, column);

	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

void sqdb_sql_alter_column(Sqdb *db, SqBuffer *buffer, SqTable *table, SqColumn *column)
{
	sq_buffer_write(buffer, "ALTER TABLE");

	// write "table name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, table->name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	if (db->info->column.use_alter) {
		// SQL Server / MS Access:
		sq_buffer_write(buffer, "ALTER COLUMN ");
		sqdb_sql_write_column(db, buffer, column, NULL);
	}
	else if (db->info->column.use_modify) {
		// My SQL / Oracle (prior version 10G):
		sq_buffer_write(buffer, "MODIFY COLUMN ");
		// Oracle 10G and later:
//		sq_buffer_write(buffer, "MODIFY ");
		sqdb_sql_write_column(db, buffer, column, NULL);
	}

	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

void sqdb_sql_rename_column(Sqdb *db, SqBuffer *buffer, SqTable *table, SqColumn *column, SqColumn *column_data)
{
	// MySQL "CHANGE COLUMN" need original column data
	if (column_data == NULL)
		column_data = column;

	sq_buffer_write(buffer, "ALTER TABLE");

	// write "table name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, table->name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	if (db->info->product != SQDB_PRODUCT_MYSQL) {
		// MySQL >= 8.0 or MariaDB >= 10.5.2 can use this
		sq_buffer_write(buffer, "RENAME COLUMN");
	}
	else {
		// MySQL < 8.0 or MariaDB < 10.5.2
		sq_buffer_write(buffer, "CHANGE COLUMN");
	}

	// write "column old name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, column->old_name);
	sq_buffer_write_c(buffer, db->info->quote.identifier[1]);

	if (db->info->product != SQDB_PRODUCT_MYSQL) {
		// MySQL >= 8.0 or MariaDB >= 10.5.2 can use this
		sq_buffer_write(buffer, " TO ");

		// write "column new name"
		sq_buffer_write_c(buffer, db->info->quote.identifier[0]);
		sq_buffer_write(buffer, column->name);
		sq_buffer_write_c(buffer, db->info->quote.identifier[1]);
	}
	else {
		// MySQL < 8.0 or MariaDB < 10.5.2
		sq_buffer_write_c(buffer, ' ');
		// write "column new name" and "column definition"
		sqdb_sql_write_column(db, buffer, column_data, column->name);
	}

	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

void  sqdb_sql_drop_column(Sqdb *db, SqBuffer *buffer, SqTable *table, SqColumn *column)
{
	if (column->type != SQ_TYPE_INDEX || db->info->product == SQDB_PRODUCT_MYSQL) {
		sq_buffer_write(buffer, "ALTER TABLE");

		// write "table name"
		sq_buffer_alloc(buffer, 2);
		sq_buffer_r_at(buffer, 1) = ' ';
		sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
		sq_buffer_write(buffer, table->name);
		sq_buffer_alloc(buffer, 2);
		sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
		sq_buffer_r_at(buffer, 0) = ' ';
	}

	sq_buffer_write(buffer, "DROP ");
	// DROP CONSTRAINT
	if (column->type == SQ_TYPE_CONSTRAINT) {
		if (db->info->product == SQDB_PRODUCT_MYSQL) {
			if (column->bit_field & SQB_COLUMN_FOREIGN || column->foreign)
				sq_buffer_write(buffer, "FOREIGN KEY");
			else if (column->bit_field & SQB_COLUMN_PRIMARY)
				sq_buffer_write(buffer, "PRIMARY KEY");
			else if (column->bit_field & SQB_COLUMN_UNIQUE)
				sq_buffer_write(buffer, "INDEX");
		}
		else
			sq_buffer_write(buffer, "CONSTRAINT");
	}
	// DROP INDEX
	else if (column->type == SQ_TYPE_INDEX) {
		sq_buffer_write(buffer, "INDEX");
		// avoid error - SQLite: no such index
		if (db->info->product == SQDB_PRODUCT_SQLITE)
			sq_buffer_write(buffer, " IF EXISTS ");
	}
	// DROP COLUMN
	else
		sq_buffer_write(buffer, "COLUMN");

	if (db->info->product != SQDB_PRODUCT_MYSQL || (column->bit_field & SQB_COLUMN_PRIMARY) == 0) {
		// write "column old name"
		sq_buffer_alloc(buffer, 2);
		sq_buffer_r_at(buffer, 1) = ' ';
		sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
		sq_buffer_write(buffer, column->old_name);
		sq_buffer_write_c(buffer, db->info->quote.identifier[1]);
	}
	buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
}

void sqdb_sql_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column)
{
	sq_buffer_write(sql_buf, "CREATE INDEX");

	// write "column name"
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ' ';
	sq_buffer_r_at(sql_buf, 0) = db->info->quote.identifier[0];
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = db->info->quote.identifier[0];
	sq_buffer_r_at(sql_buf, 0) = ' ';

	sq_buffer_write(sql_buf, "ON");

	// write "table name"
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ' ';
	sq_buffer_r_at(sql_buf, 0) = db->info->quote.identifier[0];
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write_c(sql_buf, db->info->quote.identifier[1]);

	sqdb_sql_write_composite_columns(db, sql_buf, column);

	sql_buf->mem[sql_buf->writed] = 0;    // NULL-termainated is not counted in length
}

// ----------------------------------------------------------------------------
// write parameter,arguments to 'buffer'

void sqdb_sql_write_column(Sqdb *db, SqBuffer *buffer, SqColumn *column, const char* column_name)
{
	const SqType *type;
	int     len;
	int16_t size, digits;

	if (column_name == NULL)
		column_name = column->name;
	// "column name"
	sq_buffer_write_c(buffer, db->info->quote.identifier[0]);
	sq_buffer_write(buffer, column_name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	type = column->type;
	if (SQ_TYPE_NOT_BUILTIN(type))
		type = SQ_TYPE_STRING;
	size = column->size;
	digits = column->digits;

	switch (SQ_TYPE_BUILTIN_INDEX(type)) {
	case SQ_TYPE_BOOL_INDEX:
		if (db->info->column.has_boolean)
			sq_buffer_write(buffer, "BOOLEAN");
		else
			sq_buffer_write(buffer, "TINYINT");
		break;

	case SQ_TYPE_INT_INDEX:
	case SQ_TYPE_UINT_INDEX:
	case SQ_TYPE_INTPTR_INDEX:
		if (column->bit_field & SQB_COLUMN_PRIMARY && db->info->product == SQDB_PRODUCT_SQLITE)
			sq_buffer_write(buffer, "INTEGER PRIMARY KEY");
		else if (column->bit_field & SQB_COLUMN_AUTOINCREMENT && db->info->product == SQDB_PRODUCT_POSTGRE)
			sq_buffer_write(buffer, "SERIAL");
		else {
			sq_buffer_write(buffer, "INT");
			if (size > 0) {
				len = snprintf(NULL, 0, "(%d)", size);
				sprintf(sq_buffer_alloc(buffer, len), "(%d)", size);
			}
			if (column->type == SQ_TYPE_UINT)  // || column->type == SQ_TYPE_UINTPTR
				sq_buffer_write(buffer, " UNSIGNED");
		}
		break;

	case SQ_TYPE_INT64_INDEX:
	case SQ_TYPE_UINT64_INDEX:
		if (column->bit_field & SQB_COLUMN_PRIMARY && db->info->product == SQDB_PRODUCT_SQLITE)
			sq_buffer_write(buffer, "INTEGER PRIMARY KEY");
		else if (column->bit_field & SQB_COLUMN_AUTOINCREMENT && db->info->product == SQDB_PRODUCT_POSTGRE)
			sq_buffer_write(buffer, "BIGSERIAL");
		else
			sq_buffer_write(buffer, "BIGINT");
		if (size > 0) {
			len = snprintf(NULL, 0, "(%d)", size);
			sprintf(sq_buffer_alloc(buffer, len), "(%d)", size);
		}
		if (column->type == SQ_TYPE_UINT64)
			sq_buffer_write(buffer, " UNSIGNED");
		break;

	case SQ_TYPE_TIME_INDEX:
		sq_buffer_write(buffer, "TIMESTAMP");
		break;

	case SQ_TYPE_DOUBLE_INDEX:
		sq_buffer_write(buffer, "DOUBLE");    // FLOAT
		if (size > 0 || digits > 0) {
			sq_buffer_write_c(buffer, '(');
			if (size > 0 && digits == 0) {
				// precision (total digits)
				len = snprintf(NULL, 0, "%d", size);
				sprintf(sq_buffer_alloc(buffer, len), "%d", size);
			}
			else {
				// precision (total digits) , scale (decimal digits)
				len = snprintf(NULL, 0, "%d,%d", size, digits);
				sprintf(sq_buffer_alloc(buffer, len), "%d,%d", size, digits);
			}
			sq_buffer_write_c(buffer, ')');
		}
		break;

	case SQ_TYPE_STRING_INDEX:
		size = (size <= 0) ? SQL_STRING_LENGTH_DEFAULT : size;
		len = snprintf(NULL, 0, "VARCHAR(%d)", size);
		sprintf(sq_buffer_alloc(buffer, len), "VARCHAR(%d)", size);
		break;

	/* SQ_TYPE for SQL */
	case SQ_TYPE_CHAR_INDEX:
		size = (size <= 0) ? SQL_STRING_LENGTH_DEFAULT : size;
		len = snprintf(NULL, 0, "CHAR(%d)", size);
		sprintf(sq_buffer_alloc(buffer, len), "CHAR(%d)", size);
		break;
	}

	// "NOT NULL"
	if ((column->bit_field & SQB_COLUMN_NULLABLE) == 0) {
		if (db->info->product != SQDB_PRODUCT_SQLITE || (column->bit_field & SQB_COLUMN_PRIMARY) == 0)
			sq_buffer_write(buffer, " NOT NULL");
	}
	// "AUTOINCREMENT"
	if (column->bit_field & SQB_COLUMN_AUTOINCREMENT) {
		switch (db->info->product) {
		case SQDB_PRODUCT_MYSQL:
			sq_buffer_write(buffer, " AUTO_INCREMENT");
			break;

		case SQDB_PRODUCT_POSTGRE:
			break;

		default:
			// SQLite, Access
			sq_buffer_write(buffer, " AUTOINCREMENT");
		}
	}
/*
	if (db->info->product != SQDB_PRODUCT_MYSQL) {
		// "FOREIGN KEY"
		if (column->foreign) {
			sq_buffer_write(buffer, " FOREIGN KEY");
			sqdb_sql_write_foreign_ref(db, buffer, column);
		}
		// "PRIMARY KEY"
		else if (column->bit_field & SQB_COLUMN_PRIMARY)
			sq_buffer_write(buffer, " PRIMARY KEY");
		// "UNIQUE"
		else if (column->bit_field & SQB_COLUMN_UNIQUE)
			sq_buffer_write(buffer, " UNIQUE");
	}
 */
	if (column->default_value) {
		sq_buffer_write(buffer, " DEFAULT ");
		sq_buffer_write(buffer, column->default_value);
	}
	else if (column->bit_field & SQB_COLUMN_CURRENT) {
		sq_buffer_write(buffer, " DEFAULT ");
		sq_buffer_write(buffer, "CURRENT_TIMESTAMP");
	}

	// MySQL only: ON UPDATE CURRENT_TIMESTAMP
	if (db->info->product == SQDB_PRODUCT_MYSQL && column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE) {
		sq_buffer_write(buffer, " ON UPDATE ");
		sq_buffer_write(buffer, "CURRENT_TIMESTAMP");
	}

	// raw SQL
	if (column->raw) {
		sq_buffer_write_c(buffer, ' ');
		sq_buffer_write(buffer, column->raw);
	}
}

void sqdb_sql_write_constraint(Sqdb *db, SqBuffer *buffer, SqColumn *column)
{
	// CONSTRAINT "column name" PRIMARY KEY ("col1", "col2")
	sq_buffer_write(buffer, " CONSTRAINT");

	// write "column name"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, column->name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ' ';

	if (column->foreign)
		sq_buffer_write(buffer, "FOREIGN KEY");
	else if (column->bit_field & SQB_COLUMN_PRIMARY)
		sq_buffer_write(buffer, "PRIMARY KEY");
	else if (column->bit_field & SQB_COLUMN_UNIQUE)
		sq_buffer_write(buffer, "UNIQUE");

	sqdb_sql_write_composite_columns(db, buffer, column);

	// FOREIGN KEY REFERENCES
	if (column->foreign)
		sqdb_sql_write_foreign_ref(db, buffer, column);
}

void sqdb_sql_write_composite_columns(Sqdb *db, SqBuffer *sql_buf, SqColumn *column)
{
	char **element;

	sq_buffer_write(sql_buf, " (");
	for (element = column->composite;  *element;  element++) {
		if (element != column->composite)
			sq_buffer_write_c(sql_buf, ',');
		// write "composite column"
		sq_buffer_alloc(sql_buf, 2);
		sq_buffer_r_at(sql_buf, 1) = ' ';
		sq_buffer_r_at(sql_buf, 0) = db->info->quote.identifier[0];
	 	sq_buffer_write(sql_buf, *element);
		sq_buffer_write_c(sql_buf, db->info->quote.identifier[1]);
	}
	sq_buffer_write_c(sql_buf, ')');
}

void sqdb_sql_write_foreign_ref(Sqdb *db, SqBuffer *buffer, SqColumn *column)
{
	// REFERENCES "foreign table name"("foreign column name")
	sq_buffer_write(buffer, " REFERENCES");

	// write "column foreign table"
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = db->info->quote.identifier[0];
	sq_buffer_write(buffer, column->foreign->table);    // foreign table name
	sq_buffer_write_c(buffer, db->info->quote.identifier[1]);

	// write "column foreign column"
	sq_buffer_write(buffer, " ( ");
	sq_buffer_write_c(buffer, db->info->quote.identifier[0]);
	sq_buffer_write(buffer, column->foreign->column);   // foreign column name
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = db->info->quote.identifier[1];
	sq_buffer_r_at(buffer, 0) = ')';

	// ON DELETE
	if (column->foreign->on_delete) {
		sq_buffer_write(buffer, " ON DELETE ");
		sq_buffer_write(buffer, column->foreign->on_delete);
	}
	// ON UPDATE
	if (column->foreign->on_update) {
		sq_buffer_write(buffer, " ON UPDATE ");
		sq_buffer_write(buffer, column->foreign->on_update);
	}
}

