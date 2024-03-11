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

#include <sqxclib.h>
#include <SqxcFile.h>     // SqxcFile in sqxcsupport library

/*	Sqxc chain data flow for SqxcJsonc Writer

	output ---------> SqxcJsonc Writer ---------> SqxcFile Writer
	     SQXC_TYPE_XXXX              SQXC_TYPE_STR
 */

// create and write JSON file by using C++ language
void json_file_writer_cpp()
{
	Sq::Xc            *xc;
	Sq::XcFileWriter  *xcfile;
	Sq::XcJsoncWriter *xcjson;

	xcfile = new Sq::XcFileWriter();
	xcjson = new Sq::XcJsoncWriter();
	xcfile->insert(xcjson);

	// specify output filename
	xcfile->filename = "xc_json_file_cpp.json";

	// --- Sqxc chain ready to work ---
	xcfile->ready();

	// Because arguments in xcfile never used in sqxc chain,
	// I use xcfile as arguments source here.
	xc = (Sq::Xc*)xcfile;

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT;
	xc->value.pointer = NULL;
	xcjson->send(xc);

	xc->name = "id";
	xc->type = SQXC_TYPE_INT;
	xc->value.integer = 10;
	xcjson->send(xc);

	xc->name = "name";
	xc->type = SQXC_TYPE_STR;
	xc->value.str = "bob";
	xcjson->send(xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	xc->value.pointer = NULL;
	xcjson->send(xc);

	// --- Sqxc chain finish work ---
	xcfile->finish();

	// free xcfile and xcjson in Sqxc chain
	xcfile->freeChain();
}

// create and write JSON file by using C language
void json_file_writer_c(void)
{
	Sqxc       *xc;
	SqxcFile   *xcfile;
	SqxcJsonc  *xcjson;

	xcjson = (SqxcJsonc*) sqxc_new(SQXC_INFO_JSONC_WRITER);
	xcfile = (SqxcFile*)  sqxc_new(SQXC_INFO_FILE_WRITER);
	sqxc_insert((Sqxc*)xcfile, (Sqxc*)xcjson, -1);

	// specify output filename
	xcfile->filename = "xc_json_file_c.json";

	// --- Sqxc chain ready to work ---
	sqxc_ready((Sqxc*)xcfile, NULL);

	// Because arguments in xcfile never used in sqxc chain,
	// I use xcfile as arguments source here.
	xc = (Sqxc*)xcfile;

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT;
	xc->value.pointer = NULL;
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = "id";
	xc->type = SQXC_TYPE_INT;
	xc->value.integer = 11;
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = "name";
	xc->type = SQXC_TYPE_STR;
	xc->value.str = "johny";
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	xc->value.pointer = NULL;
	sqxc_send_to((Sqxc*)xcjson, xc);

	// --- Sqxc chain finish work ---
	sqxc_finish((Sqxc*)xcfile, NULL);

	// free xcfile and xcjson in Sqxc chain
	sqxc_free_chain((Sqxc*)xcfile);
}

/*	Sqxc chain data flow for SqxcJsonc Parser

	input ---------> SqxcJsonc Parser ---------> SqxcValue
	    SQXC_TYPE_STR               SQXC_TYPE_XXXX
 */

// Defines structure to parse file written by json_file_writer_c()
typedef struct JsonTest    JsonTest;

struct JsonTest
{
	int    id;
	char*  name;
};

// define JsonTestEntries for JsonTestPointers
static const SqEntry  JsonTestEntries[] = {
	{SQ_TYPE_INT,    "id",     offsetof(JsonTest, id),      0},
	{SQ_TYPE_STR,    "name",   offsetof(JsonTest, name),    0},
};

// define JsonTestPointers for SQ_TYPE_JSON_TEST
static const SqEntry *JsonTestPointers[] = {
	&JsonTestEntries[0],
	&JsonTestEntries[1],
};

// define SQ_TYPE_JSON_TEST
static const SqType        typeJsonTest = SQ_TYPE_INITIALIZER(JsonTest, JsonTestPointers, 0);
#define SQ_TYPE_JSON_TEST &typeJsonTest

// read and parse JSON file by using C language
void json_file_parser_c(void)
{
	Sqxc       *xc;
	SqxcValue  *xcvalue;
	SqxcJsonc  *xcjson;
	JsonTest   *instance;
	FILE       *file;
	char       *buf;
	int         count;

	// open input file
	file = fopen("xc_json_file_c.json", "r");
	if (file == NULL)
		return;

	xcjson  = (SqxcJsonc*) sqxc_new(SQXC_INFO_JSONC_PARSER);
	xcvalue = (SqxcValue*) sqxc_new(SQXC_INFO_VALUE);
	sqxc_insert((Sqxc*)xcvalue, (Sqxc*)xcjson, -1);

	// setup SqxcValue
	xcvalue->container = NULL;
	xcvalue->element   = SQ_TYPE_JSON_TEST;
	xcvalue->instance  = NULL;

	// --- Sqxc chain ready to work ---
	sqxc_ready((Sqxc*)xcvalue, NULL);

	// Because arguments in xcvalue never used in sqxc chain,
	// I use xcvalue as arguments source here.
	xc = (Sqxc*)xcvalue;

	// read file data and send them to SqxcJsonc Parser
	buf = (char*)malloc(4096);
	xc->name = NULL;
	xc->type = SQXC_TYPE_STR;
	xc->value.str = buf;
	for (;;) {
		count = fread(buf, 1, 4096-1, file);
		buf[count] = 0;
		sqxc_send_to((Sqxc*)xcjson, xc);
		if (count < 4096-1)
			break;
	}
	free(buf);

	// --- Sqxc chain finish work ---
	sqxc_finish((Sqxc*)xcvalue, NULL);

	// get instance of SQ_TYPE_JSON_TEST
	instance = (JsonTest*)xcvalue->instance;
	printf("id = %d, name = %s\n", instance->id, instance->name);
	free(instance->name);
	free(instance);

	// free xcvalue and xcjson in Sqxc chain
	sqxc_free_chain((Sqxc*)xcvalue);

	// close input file
	fclose(file);
}

int main(void)
{
	json_file_writer_c();
	json_file_writer_cpp();

	json_file_parser_c();

	return EXIT_SUCCESS;
}
