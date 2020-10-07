/*
 *   Copyright (C) 2020 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <sqdb.h>

Sqdb*   sqdb_new(const SqdbInfo *info, SqdbConfig* config)
{
	void (*init)(Sqdb* db, SqdbConfig* conf);
	Sqdb* db;

	if (info == NULL)
		info = config->info;
	init = info->init;
	if (init) {
		db = malloc(info->size);
		info->init(db, config);
	}
	else
		db = calloc(1, info->size);
	db->info = info;
	return db;
}

void    sqdb_free(Sqdb* db)
{
	void (*final)(Sqdb* db);

	final = db->info->final;
	if (final)
		final(db);
	free(db);
}

// ----------------------------------------------------------------------------

#define DEFAULT_STRING_LENGTH    191

static int  sqdb_create_table_reo(Sqdb* db, SqBuffer* buffer, SqSchema* schema, SqTable* table);

static void create_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqPtrArray* arranged_columns);
static void rename_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table);
static void alter_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqPtrArray* arranged_columns);
static void drop_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table);

static bool add_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column);
static bool rename_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column);
static bool alter_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column);
static bool drop_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column);

static void column_type_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column);
static void constraint_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column);
static void foreign_ref_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column);


void sqdb_schema_to_sql(Sqdb* db, SqBuffer* buffer, SqSchema* schema, SqPtrArray* arranged_tables)
{
	SqTable* table;

	if (arranged_tables == NULL)
		arranged_tables = sq_type_get_ptr_array(schema->type);
	for (int index = 0;  index < arranged_tables->length;  index++) {
		table = (SqTable*)arranged_tables->data[index];

		if (index > 0)
			sq_buffer_write_c(buffer, ' ');

		if (table->bit_field & SQB_CHANGE) {
			// ALTER TABLE
			alter_table_to_sql(db, buffer, table, NULL);
		}
		else if (table->name == NULL) {
			// DROP TABLE
			drop_table_to_sql(db, buffer, table);
		}
		else if (table->old_name) {
			// RENAME TABLE
			rename_table_to_sql(db, buffer, table);
		}
		else {
			// CREATE TABLE
			if ((table->bit_field & SQB_TABLE_SQL_CREATED) == 0)
				sqdb_create_table_reo(db, buffer, schema, table);
			if (table->foreigns.length > 0)
				alter_table_to_sql(db, buffer, table, &table->foreigns);
		}
	}
}

// ------------------------------------
// SqTable

// sort schema->type->entry by table->name before calling this function.
int  sqdb_create_table_reo(Sqdb* db, SqBuffer* buffer, SqSchema* schema, SqTable* table)
{
	SqColumn*   column;
	SqPtrArray* arranged;
	SqPtrArray  tmp_array = (SqPtrArray){NULL, 0};
	int         code = SQCODE_OK;

	// SQLite
	if (db->info->product == SQDB_PRODUCT_SQLITE && table->bit_field & SQB_TABLE_REO_CONSTRAINT) {
		// avoid infinite recursive
		table->bit_field |= SQB_TABLE_REO_CHECKING;
		// check constraint reference each other
		for (int index = 0;  index < table->foreigns.length;  index++) {
			column = table->foreigns.data[index];
			if (column->constraint) {
				SqTable* fore_table;

//				fore_table = sq_type_find_entry(schema->type, column->foreign->table, NULL);
				fore_table = sq_ptr_array_search(&schema->type->entry, column->foreign->table,
		                                         (SqCompareFunc)sq_entry_cmp_str__name);
				if (fore_table) {
					fore_table = *(SqTable**)fore_table;
					if (fore_table->bit_field & SQB_TABLE_SQL_CREATED)
						continue;
					if (fore_table->bit_field & SQB_TABLE_REO_CHECKING) {
						// error...
						// constraint reference each other
						code = SQCODE_REFERENCE_EACH_OTHER;
						break;
					}
				}
				else {
					// error...
					code = SQCODE_REFERENCE_NOT_FOUND;
					continue;
				}
				// recursive
				sqdb_create_table_reo(db, buffer, schema, fore_table);
				// remove current column in excluded list
				table->foreigns.data[index] = NULL;
			}
		}
		sq_reentries_remove_null(&table->foreigns);
		// avoid infinite recursive
		table->bit_field &= ~SQB_TABLE_REO_CHECKING;
	}

	if (table->foreigns.length == 0)
		arranged = sq_type_get_ptr_array(table->type);
	else {
		sq_ptr_array_init(&tmp_array, 16, NULL);
		sq_table_exclude(table, &table->foreigns, &tmp_array);
		arranged = &tmp_array;
	}

	// move primary key to front of table and move constraint to end of table
	sq_ptr_array_sort(arranged, (SqCompareFunc)sq_column_cmp_attrib);
	// SQL create table
	create_table_to_sql(db, buffer, table, arranged);
	table->bit_field |= SQB_TABLE_SQL_CREATED;

	if (tmp_array.data)
		sq_ptr_array_final(&tmp_array);

	return code;
}

// ----------------------------------------------------------------------------

static void create_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqPtrArray* arranged_columns)
{
	SqColumn* column;
	int       index;
	bool      has_constraint = false;

	sq_buffer_write(buffer, "CREATE TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" (");

	if (arranged_columns == NULL)
		arranged_columns = sq_type_get_ptr_array(table->type);
	for (index = 0;  index < arranged_columns->length;  index++) {
		column = (SqColumn*)arranged_columns->data[index];
		// skip ignore
//		if (column->bit_field & SQB_IGNORE)
//			continue;
		// skip "dropped" or "renamed"
		if (column->old_name)
			continue;
		// skip constraint
		if (column->constraint) {
			has_constraint = true;
			continue;
		}
		if (index > 0) {
			sq_buffer_alloc(buffer, 2);
			buffer->buf[buffer->writed -2] = ',';
			buffer->buf[buffer->writed -1] = ' ';
		}
		column_type_to_sql(db, buffer, column);
	}

	if (db->info->product == SQDB_PRODUCT_MYSQL) {
		for (index = 0;  index < arranged_columns->length;  index++) {
			column = (SqColumn*)arranged_columns->data[index];
			// Don't output CONSTRAINT here
			if (column->constraint)
				continue;
			// FOREIGN KEY
			if (column->foreign)
				sq_buffer_write(buffer, ", FOREIGN KEY");
			// PRIMARY KEY
			else if (column->bit_field & SQB_PRIMARY)
				sq_buffer_write(buffer, ", PRIMARY KEY");
			// UNIQUE
			else if (column->bit_field & SQB_UNIQUE)
				sq_buffer_write(buffer, ", UNIQUE");
			else
				continue;
			// ("column_name")
			sq_buffer_write(buffer, " (\"");
			sq_buffer_write(buffer, column->name);
			sq_buffer_alloc(buffer, 2);
			buffer->buf[buffer->writed -2] = '"';
			buffer->buf[buffer->writed -1] = ')';

			// FOREIGN KEY REFERENCES
			if (column->foreign)
				foreign_ref_to_sql(db, buffer, column);
		}
	}

	if (has_constraint) {
		for (index = 0;  index < arranged_columns->length;  index++) {
			column = (SqColumn*)arranged_columns->data[index];
			if (column->name == NULL)
				continue;
			// CONSTRAINT
			if (column->constraint) {
				sq_buffer_write_c(buffer, ',');
				constraint_to_sql(db, buffer, column);
			}
		}
	}

	sq_buffer_alloc(buffer, 2);
	buffer->buf[buffer->writed -2] = ')';
	buffer->buf[buffer->writed -1] = ';';
}

static void drop_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table)
{
	// DROP TABLE "name";
	sq_buffer_write(buffer, "DROP TABLE \"");
	strcpy(sq_buffer_alloc(buffer, strlen(table->old_name) +2), table->old_name);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ';';
}

static void rename_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table)
{
	// ALTER TABLE "old_name" RENAME "new_name";
	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->old_name);
//	sq_buffer_write_c(buffer, '"');
	buffer->buf[buffer->writed++] = '"';

	sq_buffer_write(buffer, " RENAME \"");
	strcpy(sq_buffer_alloc(buffer, strlen(table->name) +2), table->name);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ';';
}

static void alter_table_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqPtrArray* arranged_columns)
{
	SqColumn* column;
	int       index;
	bool      is_ok;

	if (arranged_columns == NULL)
		arranged_columns = sq_type_get_ptr_array(table->type);
	// ALTER TABLE
	for (index = 0;  index < arranged_columns->length;  index++) {
		column = (SqColumn*)arranged_columns->data[index];
//		if (column->bit_field & SQB_IGNORE)
//			continue;
		if (column->bit_field & SQB_CHANGE) {
			// ALTER COLUMN
			is_ok = alter_column_to_sql(db, buffer, table, column);
		}
		else if (column->name == NULL) {
			// DROP COLUMN / CONSTRAINT / KEY
			is_ok = drop_column_to_sql(db, buffer, table, column);
		}
		else if (column->old_name) {
			// RENAME COLUMN
			is_ok = rename_column_to_sql(db, buffer, table, column);
		}
		else {
			// ADD COLUMN / CONSTRAINT / KEY
			is_ok = add_column_to_sql(db, buffer, table, column);
		}

		if (is_ok)
			sq_buffer_write_c(buffer, ';');
	}

}

// ------------------------------------
// column

static void alter_table_add_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table)
{
	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	sq_buffer_write(buffer, "ADD ");
}

static bool add_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column)
{
	// ADD CONSTRAINT
	if (column->constraint) {
		// SQLite doesn't support this
		if (db->info->product != SQDB_PRODUCT_SQLITE)
			return false;
		alter_table_add_to_sql(db, buffer, table);
		constraint_to_sql(db, buffer, column);
		return true;
	}
	// ADD FOREIGN KEY
	else if (column->foreign) {
		// SQLite doesn't support this
		if (db->info->product != SQDB_PRODUCT_SQLITE)
			return false;
		alter_table_add_to_sql(db, buffer, table);
		sq_buffer_write(buffer, "FOREIGN KEY");
	}
	// ADD PRIMARY KEY
	else if (column->bit_field & SQB_PRIMARY) {
		// SQLite doesn't support this
		if (db->info->product != SQDB_PRODUCT_SQLITE)
			return false;
		alter_table_add_to_sql(db, buffer, table);
		sq_buffer_write(buffer, "PRIMARY KEY");
	}
	// ADD UNIQUE
	else if (column->bit_field & SQB_UNIQUE) {
		// SQLite doesn't support this
		if (db->info->product != SQDB_PRODUCT_SQLITE)
			return false;
		alter_table_add_to_sql(db, buffer, table);
		sq_buffer_write(buffer, "UNIQUE");
	}
	// ADD COLUMN
	else {
		alter_table_add_to_sql(db, buffer, table);
		column_type_to_sql(db, buffer, column);
		return true;
	}

	// ("column_name")
	sq_buffer_write(buffer, " (\"");
	sq_buffer_write(buffer, column->name);
	sq_buffer_alloc(buffer, 2);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ')';

	// ADD FOREIGN KEY REFERENCES
	if (column->foreign)
		foreign_ref_to_sql(db, buffer, column);

	return true;
}

static bool alter_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column)
{
	if (db->info->product == SQDB_PRODUCT_SQLITE) {
		// SQLite doesn't support ALTER COLUMN
		return false;
	}

	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	if (db->info->column.use_alter) {
		// SQL Server / MS Access:
		sq_buffer_write(buffer, "ALTER COLUMN ");
		column_type_to_sql(db, buffer, column);
	}
	else if (db->info->column.use_modify) {
		// My SQL / Oracle (prior version 10G):
		sq_buffer_write(buffer, "MODIFY COLUMN ");
		// Oracle 10G and later:
//		sq_buffer_write(buffer, "MODIFY ");
		column_type_to_sql(db, buffer, column);
	}
	return true;
}

static bool rename_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column)
{
	int  len;

	if (db->info->product == SQDB_PRODUCT_SQLITE) {
		// SQLite doesn't support RENAME COLUMN statement
		// SQlite >= 3.20.0 support it.
		return false;
	}
	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	sq_buffer_write(buffer, "RENAME COLUMN ");
	len = snprintf(NULL, 0, "\"%s\" TO \"%s\"",
	               column->old_name, column->name);
	sprintf(sq_buffer_alloc(buffer, len), "\"%s\" TO \"%s\"",
	        column->old_name, column->name);
	return true;
}

static bool drop_column_to_sql(Sqdb* db, SqBuffer* buffer, SqTable* table, SqColumn* column)
{
	if (db->info->product == SQDB_PRODUCT_SQLITE) {
		// SQLite doesn't support DROP
		return false;
	}

	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	sq_buffer_write(buffer, "DROP");
	// DROP CONSTRAINT
	if (column->constraint || column->bit_field & SQB_CONSTRAINT) {
		if (db->info->product == SQDB_PRODUCT_MYSQL) {
			if (column->bit_field & SQB_FOREIGN || column->foreign)
				sq_buffer_write(buffer, " FOREIGN KEY");
			else if (column->bit_field & SQB_PRIMARY)
				sq_buffer_write(buffer, " PRIMARY KEY");
			else if (column->bit_field & SQB_UNIQUE)
				sq_buffer_write(buffer, " INDEX");
		}
		else
			sq_buffer_write(buffer, " CONSTRAINT");
	}
	// DROP COLUMN
	else {
		sq_buffer_write(buffer, " COLUMN");
	}

	sq_buffer_write(buffer, " \"");
	sq_buffer_write(buffer, column->old_name);
	sq_buffer_write_c(buffer, '"');
	return true;
}

static void column_type_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column)
{
	SqType* type;
	int     len;
	int16_t size, digits;

	// "column_name"
	sq_buffer_write_c(buffer, '"');
	sq_buffer_write(buffer, column->name);
	sq_buffer_write(buffer, "\" ");
	type = column->type;
	if (SQ_TYPE_NOT_BUILTIN(type))
		type = SQ_TYPE_STRING;
	size = column->size;
	digits = column->digits;

	switch (SQ_TYPE_BUILTIN_INDEX(type)) {
	case SQ_TYPE_INDEX_INT:
	case SQ_TYPE_INDEX_UINT:
	case SQ_TYPE_INDEX_INTPTR:
		sq_buffer_write(buffer, "INT");
		if (size > 0) {
			len = snprintf(NULL, 0, "(%d)", size);
			sprintf(sq_buffer_alloc(buffer, len), "(%d)", size);
		}
		if (column->type == SQ_TYPE_UINT)  // || column->type == SQ_TYPE_UINTPTR
			sq_buffer_write(buffer, " UNSIGNED");
		break;

	case SQ_TYPE_INDEX_INT64:
	case SQ_TYPE_INDEX_UINT64:
		sq_buffer_write(buffer, "BIGINT");
		if (size > 0) {
			len = snprintf(NULL, 0, "(%d)", size);
			sprintf(sq_buffer_alloc(buffer, len), "(%d)", size);
		}
		if (column->type == SQ_TYPE_UINT64)
			sq_buffer_write(buffer, " UNSIGNED");
		break;

	case SQ_TYPE_INDEX_TIME:
		sq_buffer_write(buffer, "TIMESTAMP");
		break;

	case SQ_TYPE_INDEX_DOUBLE:
		sq_buffer_write(buffer, "DOUBLE");    // FLOAT
		if (size > 0 || digits > 0) {
			sq_buffer_write_c(buffer, '(');
			if (size > 0 && digits == 0) {
				len = snprintf(NULL, 0, "%d", size);
				sprintf(sq_buffer_alloc(buffer, len), "%d", size);
			}
			else {
				len = snprintf(NULL, 0, "%d,%d", size, digits);
				sprintf(sq_buffer_alloc(buffer, len), "%d,%d", size, digits);
			}
			sq_buffer_write_c(buffer, ')');
		}
		break;

	case SQ_TYPE_INDEX_STRING:
		size = (size <= 0) ? DEFAULT_STRING_LENGTH : size;
		len = snprintf(NULL, 0, "VARCHAR(%d)", size);
		sprintf(sq_buffer_alloc(buffer, len), "VARCHAR(%d)", size);
		break;
	}

	// "AUTOINCREMENT"
	if (column->bit_field & SQB_AUTOINCREMENT)
		sq_buffer_write(buffer, " AUTOINCREMENT");
	// "NOT NULL"
	if ((column->bit_field & SQB_NULLABLE) == 0)
		sq_buffer_write(buffer, " NOT NULL");

	if (db->info->product != SQDB_PRODUCT_MYSQL) {
		// "FOREIGN KEY"
		if (column->foreign) {
			sq_buffer_write(buffer, " FOREIGN KEY");
			foreign_ref_to_sql(db, buffer, column);
		}
		// "PRIMARY KEY"
		else if (column->bit_field & SQB_PRIMARY)
			sq_buffer_write(buffer, " PRIMARY KEY");
		// "UNIQUE"
		else if (column->bit_field & SQB_UNIQUE)
			sq_buffer_write(buffer, " UNIQUE");
	}

	if (column->default_value) {
		sq_buffer_write(buffer, " DEFAULT ");
		sq_buffer_write(buffer, column->default_value);
	}

	// raw SQL
	if (column->extra) {
		sq_buffer_write_c(buffer, ' ');
		sq_buffer_write(buffer, column->extra);
	}
}

static void constraint_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column)
{
	char** constraint;

	// CONSTRAINT "column name" PRIMARY KEY ("col1", "col2")
	sq_buffer_write(buffer, " CONSTRAINT \"");
	sq_buffer_write(buffer, column->name);
	sq_buffer_write_c(buffer, '"');

	if (column->foreign)
		sq_buffer_write(buffer, " FOREIGN KEY");
	else if (column->bit_field & SQB_PRIMARY)
		sq_buffer_write(buffer, " PRIMARY KEY");
	else if (column->bit_field & SQB_UNIQUE)
		sq_buffer_write(buffer, " UNIQUE");

	sq_buffer_write(buffer, " (\"");
	for (constraint = column->constraint;  *constraint;  constraint++) {
		if (constraint != column->constraint)
			sq_buffer_write(buffer, ",\"");
	 	sq_buffer_write(buffer, *constraint);
		sq_buffer_write_c(buffer, '"');
	}
	sq_buffer_write_c(buffer, ')');

	// FOREIGN KEY REFERENCES
	if (column->foreign)
		foreign_ref_to_sql(db, buffer, column);
}

static void foreign_ref_to_sql(Sqdb* db, SqBuffer* buffer, SqColumn* column)
{
	// REFERENCES "foreign table name"("foreign column name")
	sq_buffer_write(buffer, " REFERENCES \"");
	sq_buffer_write(buffer, column->foreign->table);    // foreign table name
	sq_buffer_write(buffer, "\"(\"");
	sq_buffer_write(buffer, column->foreign->column);   // foreign column name
	sq_buffer_alloc(buffer, 2);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ')';
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

