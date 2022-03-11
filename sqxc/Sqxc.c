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

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqConfig.h>
#include <SqError.h>
#include <Sqxc.h>

/*	after sqxc_init():
	Sqxc.nested_count = 0
	Sqxc.nested       = SQXC_NESTED_EMPTY

	Sqxc.nested_count used by sqxc_push_nested() and sqxc_pop_nested(). They use it to alloc/free chunk.
	SQXC_NESTED_EMPTY is used to reduce "If Statements".
 */
static const SqxcNested SqxcNested_Empty_ = {0};

#define SQXC_NESTED_EMPTY    (&SqxcNested_Empty_)

// ----------------------------------------------------------------------------
// Sqxc functions

void   sqxc_init(Sqxc *xc, const SqxcInfo *xcinfo)
{
	SqInitFunc init;

	memset(xc, 0, xcinfo->size);
	xc->nested = (SqxcNested*)SQXC_NESTED_EMPTY;
	init = xcinfo->init;
	if (init)
		init(xc);
	xc->info = xcinfo;
}

void   sqxc_final(Sqxc *xc)
{
	SqFinalFunc final;

	final = xc->info->final;
	if (final)
		final(xc);
	// clear nested
	sqxc_clear_nested(xc);
	// free memory
	free(xc->buf);
}

Sqxc  *sqxc_new(const SqxcInfo *xcinfo)
{
	Sqxc *xc;

	xc = (Sqxc*)malloc(xcinfo->size);
	sqxc_init(xc, xcinfo);
	return xc;
}

void  sqxc_free(Sqxc *xc)
{
	sqxc_final(xc);
	free(xc);
}

Sqxc *sqxc_new_chain(const SqxcInfo *info, ...)
{
	va_list    arg_list;
	Sqxc      *xc = NULL;
	Sqxc      *cur;

	cur = xc = sqxc_new(info);

	va_start(arg_list, info);
	for (;;) {
		info = va_arg(arg_list, SqxcInfo*);
		if (info == NULL)
			break;
		// append new element
		cur->peer = sqxc_new(info);
		cur->peer->dest = xc;
		cur = cur->peer;
	};
	va_end(arg_list);

	return xc;
}

void  sqxc_free_chain(Sqxc *xc)
{
	Sqxc  *peer;

	// free Sqxc from first to last.
	for (;  xc;  xc = peer) {
		peer = xc->peer;
		sqxc_free(xc);
	}
}

Sqxc   *sqxc_insert(Sqxc *xc, Sqxc *xc_element, int position)
{
	Sqxc *cur;
	Sqxc *prev = NULL;

	for (cur = xc;  cur;  cur = cur->peer, position--) {
		if (position == 0)
			break;
		prev = cur;
	}

	// if 'xc_element' is NULL, return Sqxc element at the given position.
	if (xc_element == NULL)
		return cur;
	// insert a new Sqxc element into the chain
	if (prev)
		prev->peer = xc_element;
	if (cur)
		xc_element->peer = cur;
	// set default destination
	xc_element->dest = xc;
	return (prev) ? xc : xc_element;
}

Sqxc   *sqxc_steal(Sqxc *xc, Sqxc *xc_element)
{
	Sqxc *cur;
	Sqxc *prev = NULL;

	for (cur = xc;  cur;  cur = cur->peer) {
		if (cur == xc_element) {
			if (prev)
				prev->peer = cur->peer;
			cur->peer = NULL;
		}
		prev = cur;
	}
	return (prev) ? xc : xc->peer;
}

Sqxc   *sqxc_find(Sqxc *xc, const SqxcInfo *info)
{
	for (;  xc;  xc = xc->peer) {
		if (xc->info == info)
			return xc;
	}
	return NULL;
}

int   sqxc_broadcast(Sqxc *xc, int id, void *data)
{
	int   code = SQCODE_OK;
	Sqxc *peer;

	// call SqxcInfo.ctrl()
	for (;  xc;  xc = peer) {
		peer = xc->peer;
		if (xc->info->ctrl) {
			xc->code = xc->info->ctrl(xc, id, data);
			if (xc->code != SQCODE_OK)
				code = xc->code;
		}
	}
	return code;
}

Sqxc  *sqxc_send(Sqxc *xc)
{
	Sqxc    *cur;

/*	if (xc->error == ERROR_MUST_STOP) {
		xc->code = error_code;
		return xc;
	}
 */

/*	xc->required_type = SQXC_TYPE_ALL;  */
	for (cur = xc;  cur;  cur = cur->peer) {
		if ((cur->supported_type & xc->type) == 0) {
			xc->code = SQCODE_TYPE_NOT_SUPPORT;
			continue;
		}
/*		if ((cur->outputable_type & xc->required_type) == 0)
			continue;
 */
		// change destination before calling send()
		if (cur != xc)
			cur->dest = xc;
		xc->code = cur->info->send(cur, xc);

		if (xc->code == SQCODE_OK) {
			// change current Sqxc element
			if (cur->nested_count == 0 && cur->dest) {
				xc = cur->dest;
				cur = xc;
			}
			cur->code = SQCODE_OK;    // set code for convenient
			return cur;
		}
		else if (xc->code == SQCODE_ENTRY_NOT_FOUND)
			break;
	}

	return xc;
}

// ----------------------------------------------------------------------------
// push/pop nested object/array

#define NESTED_CHUNK_SIZE    (1 << 3)    // 8
#define NESTED_CHUNK_MASK    (NESTED_CHUNK_SIZE -1)

void  sqxc_clear_nested(Sqxc *xc)
{
	while (xc->nested != SQXC_NESTED_EMPTY)
		sqxc_pop_nested(xc);
	xc->nested_count = 0;
}

void  sqxc_erase_nested(Sqxc *xc, SqxcNested *nested)
{
	SqxcNested *inner;

	for (;  nested->inner;  nested = inner) {
		inner = nested->inner;
		nested->data  = inner->data;
		nested->data2 = inner->data2;
		nested->data3 = inner->data3;
	}
	sqxc_pop_nested(xc);
}

SqxcNested *sqxc_push_nested(Sqxc *xc)
{
	SqxcNested *outer = xc->nested;
	SqxcNested *nested;

/*	// every outer has one inner at the same time
	if (outer != SQXC_NESTED_EMPTY && outer->inner) {
		xc->code = SQCODE_TOO_MANY_NESTED;
		return NULL;
	}
 */

	// alloc multiple SqxcNested each time
	if ((xc->nested_count & NESTED_CHUNK_MASK) == 0)
		nested = (SqxcNested*)malloc(sizeof(SqxcNested) * NESTED_CHUNK_SIZE);
	else
		nested = xc->nested +1;
	xc->nested_count++;

	// link
	if (outer != SQXC_NESTED_EMPTY)
		outer->inner = nested;
	nested->outer = outer;
	nested->inner = NULL;
	xc->nested = nested;
	return nested;
}

void sqxc_pop_nested(Sqxc *xc)
{
	SqxcNested *outer;

	if (xc->nested != SQXC_NESTED_EMPTY) {
		outer = xc->nested->outer;
		if (outer != SQXC_NESTED_EMPTY)
			outer->inner = NULL;
		// free multiple SqxcNested each time
		xc->nested_count--;
		if ((xc->nested_count & NESTED_CHUNK_MASK) == 0)
			free(xc->nested);
		xc->nested = outer;
	}
}
