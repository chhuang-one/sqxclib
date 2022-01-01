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

#include <sqxclib.h>
#include <SqxcFile.h>    // SqxcFile in sqxctest library

/*	Sqxc chain data flow in this file

	program ---------> SqxcJson Writer ---------> SqxcFile Writer
	      SQXC_TYPE_XXXX            SQXC_TYPE_STRING
 */

// create and write JSON file by using C++ Language
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
	xcjson->send(xc);

	xc->name = "id";
	xc->type = SQXC_TYPE_INT;
	xc->value.integer = 10;
	xcjson->send(xc);

	xc->name = "name";
	xc->type = SQXC_TYPE_STRING;
	xc->value.string = "bob";
	xcjson->send(xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	xcjson->send(xc);

	// --- Sqxc chain finish work ---
	xcfile->finish();

	// free xcfile and xcjson in Sqxc chain
	xcfile->freeChain();
}

// create and write JSON file by using C Language
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
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = "id";
	xc->type = SQXC_TYPE_INT;
	xc->value.integer = 11;
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = "name";
	xc->type = SQXC_TYPE_STRING;
	xc->value.string = "johny";
	sqxc_send_to((Sqxc*)xcjson, xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	sqxc_send_to((Sqxc*)xcjson, xc);

	// --- Sqxc chain finish work ---
	sqxc_finish((Sqxc*)xcfile, NULL);

	// free xcfile and xcjson in Sqxc chain
	sqxc_free_chain((Sqxc*)xcfile);
}

int main(void)
{
	json_file_writer_c();
	json_file_writer_cpp();

	return EXIT_SUCCESS;
}
