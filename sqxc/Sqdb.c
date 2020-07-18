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

#include <SqBuffer.h>
#include <sqdb.h>

void sqdb_create_table(Sqdb* db, SqTable* table)
{

}

SqdbInfo dbinfo_mysql = {
	.column = {
		.mysql = 0,
		.can_alter = 1,
		.can_modify = 1,
	},
};

void sqdb_info_init_sqlite(SqdbInfo* dbinfo)
{
	dbinfo->column.mysql = 0;
	dbinfo->column.sqlite = 1;
	dbinfo->column.can_alter = 0;
	dbinfo->column.can_modify = 0;
}

void sqdb_info_init_mysql(SqdbInfo* dbinfo)
{
	dbinfo->column.mysql = 1;
	dbinfo->column.can_alter = 1;
	dbinfo->column.can_modify = 1;
}

// ----------------------------------------------------------------------------

#define DEFAULT_STRING_LENGTH    191

static void create_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer);
static void rename_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer);
static void alter_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer);
static void drop_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer);

static bool add_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer);
static bool rename_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer);
static bool alter_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer);
static bool drop_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer);

static void column_type_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer);
static void constraint_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer);
static void foreign_ref_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer);


void sqdb_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	if (table->bit_field & SQB_CHANGE) {
		// ALTER TABLE
		alter_table_to_sql(db, table, buffer);
	}
	else if (table->name == NULL) {
		// DROP TABLE
		drop_table_to_sql(db, table, buffer);
	}
	else if (table->old_name) {
		// RENAME TABLE
		rename_table_to_sql(db, table, buffer);
	}
	else {
		// CREATE TABLE
		create_table_to_sql(db, table, buffer);
	}
}


static void create_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	SqType*   type;
	SqColumn* column;
	int       index;
	bool      has_constraint = false;

	sq_buffer_write(buffer, "CREATE TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" (");

	type = table->type;
	for (index = 0;  index < type->map_length;  index++) {
		column = (SqColumn*)type->map[index];
		if (column->bit_field & SQB_IGNORE)
			continue;
		if (column->old_name || column->name == NULL)
			continue;
		if (column->constraint) {
			has_constraint = true;
			continue;
		}
		if (index > 0) {
			sq_buffer_alloc(buffer, 2);
			buffer->buf[buffer->writed -2] = ',';
			buffer->buf[buffer->writed -1] = ' ';
		}
		column_type_to_sql(db, column, buffer);
	}

	if (db->info->column.mysql == 1) {
		for (index = 0;  index < type->map_length;  index++) {
			column = (SqColumn*)type->map[index];
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
				foreign_ref_to_sql(db, column, buffer);
		}
	}

	if (has_constraint) {
		for (index = 0;  index < type->map_length;  index++) {
			column = (SqColumn*)type->map[index];
			if (column->name == NULL)
				continue;
			// CONSTRAINT
			if (column->constraint) {
				sq_buffer_write_c(buffer, ',');
				constraint_to_sql(db, column, buffer);
			}
		}
	}

	sq_buffer_alloc(buffer, 2);
	buffer->buf[buffer->writed -2] = ')';
	buffer->buf[buffer->writed -1] = ';';
}

static void drop_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	// DROP TABLE "name";
	sq_buffer_write(buffer, "DROP TABLE \"");
	strcpy(sq_buffer_alloc(buffer, strlen(table->name) +2), table->name);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ';';
}

static void rename_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	// ALTER TABLE "old_name" RENAME "new_name";
	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->old_name);
//	sq_buffer_write_c(buffer, '"');
	buffer->buf[buffer->writed++] = '"';

	sq_buffer_write(buffer, "RENAME \"");
	strcpy(sq_buffer_alloc(buffer, strlen(table->old_name) +2), table->old_name);
	buffer->buf[buffer->writed -2] = '"';
	buffer->buf[buffer->writed -1] = ';';
}

static void alter_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	SqType*   type;
	SqColumn* column;
	int       index;
	bool      is_ok;

	// ALTER TABLE
	type = table->type;
	for (index = 0;  index < type->map_length;  index++) {
		column = (SqColumn*)type->map[index];
		if (column->bit_field & SQB_IGNORE)
			continue;
		if (column->bit_field & SQB_CHANGE) {
			// ALTER COLUMN
			is_ok = alter_column_to_sql(db, table, column, buffer);
		}
		else if (column->name == NULL) {
			// DROP COLUMN / CONSTRAINT / KEY
			is_ok = drop_column_to_sql(db, table, column, buffer);
		}
		else if (column->old_name) {
			// RENAME COLUMN
			is_ok = rename_column_to_sql(db, table, column, buffer);
		}
		else {
			// ADD COLUMN / CONSTRAINT / KEY
			is_ok = add_column_to_sql(db, table, column, buffer);
		}

		if (is_ok)
			sq_buffer_write_c(buffer, ';');
	}

}

// ------------------------------------
// column

static void alter_table_add_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer)
{
	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	sq_buffer_write(buffer, "ADD ");
}

static bool add_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer)
{
	// ADD CONSTRAINT
	if (column->constraint) {
		// SQLite doesn't support this
		if (db->info->column.sqlite == 0)
			return false;
		alter_table_add_to_sql(db, table, buffer);
		constraint_to_sql(db, column, buffer);
		return true;
	}
	// ADD FOREIGN KEY
	else if (column->foreign) {
		// SQLite doesn't support this
		if (db->info->column.sqlite == 0)
			return false;
		alter_table_add_to_sql(db, table, buffer);
		sq_buffer_write(buffer, "FOREIGN KEY");
	}
	// ADD PRIMARY KEY
	else if (column->bit_field & SQB_PRIMARY) {
		// SQLite doesn't support this
		if (db->info->column.sqlite == 0)
			return false;
		alter_table_add_to_sql(db, table, buffer);
		sq_buffer_write(buffer, "PRIMARY KEY");
	}
	// ADD UNIQUE
	else if (column->bit_field & SQB_UNIQUE) {
		// SQLite doesn't support this
		if (db->info->column.sqlite == 0)
			return false;
		alter_table_add_to_sql(db, table, buffer);
		sq_buffer_write(buffer, "UNIQUE");
	}
	// ADD COLUMN
	else {
		alter_table_add_to_sql(db, table, buffer);
		column_type_to_sql(db, column, buffer);
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
		foreign_ref_to_sql(db, column, buffer);

	return true;
}

static bool rename_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer)
{
	int  len;

	if (db->info->column.sqlite) {
		// SQLite doesn't support rename column
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

static bool alter_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer)
{
	if (db->info->column.sqlite) {
		// SQLite doesn't support alter
		return false;
	}

	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	if (db->info->column.can_alter) {
		// SQL Server / MS Access:
		sq_buffer_write(buffer, "ALTER COLUMN ");
		column_type_to_sql(db, column, buffer);
	}
	else if (db->info->column.can_modify) {
		// My SQL / Oracle (prior version 10G):
		sq_buffer_write(buffer, "MODIFY COLUMN ");
		// Oracle 10G and later:
//		sq_buffer_write(buffer, "MODIFY ");
		column_type_to_sql(db, column, buffer);
	}
	return true;
}

static bool drop_column_to_sql(Sqdb* db, SqTable* table, SqColumn* column, SqBuffer* buffer)
{
	if (db->info->column.sqlite) {
		// SQLite doesn't support DROP
		return false;
	}

	sq_buffer_write(buffer, "ALTER TABLE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" ");

	sq_buffer_write(buffer, "DROP");
	// DROP CONSTRAINT
	if (column->constraint || column->bit_field & SQB_CONSTRAINT) {
		if (db->info->column.mysql) {
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
	sq_buffer_write(buffer, column->name ? column->name : column->old_name );
	sq_buffer_write_c(buffer, '"');
	return true;
}


static void column_type_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer)
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

	if (db->info->column.mysql == 0) {
		// "FOREIGN KEY"
		if (column->foreign) {
			sq_buffer_write(buffer, " FOREIGN KEY");
			foreign_ref_to_sql(db, column, buffer);
		}
		// "PRIMARY KEY"
		else if (column->bit_field & SQB_PRIMARY)
			sq_buffer_write(buffer, " PRIMARY KEY");
		// "UNIQUE"
		else if (column->bit_field & SQB_UNIQUE)
			sq_buffer_write(buffer, " UNIQUE");
	}
}

static void constraint_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer)
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
		foreign_ref_to_sql(db, column, buffer);
}

static void foreign_ref_to_sql(Sqdb* db, SqColumn* column, SqBuffer* buffer)
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

