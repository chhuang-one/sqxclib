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

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <Sqxc.h>

#define NESTED_OUTER_ROOT    (&sqxc_nested_root)

static SqxcNested sqxc_nested_root = {0};

// ----------------------------------------------------------------------------
// Sqxc functions

Sqxc*  sqxc_new(const SqxcInfo* xcinfo, int io)
{
	Sqxc* xc;
	SqInitFunc init;

	io &= 1;    // 0 or 1
	xc = (Sqxc*)calloc(1, xcinfo[io].size);
	xc->nested = NESTED_OUTER_ROOT;
	init = xcinfo[io].init;
	if (init)
		init(xc);
	xc->info = xcinfo;
	xc->io_ = io;
	return xc;
}

void  sqxc_free(Sqxc* xc)
{
	SqFinalFunc final;

	final = xc->info[xc->io_].final;
	if (final)
		final(xc);
	// free memory
	while (xc->nested != NESTED_OUTER_ROOT)
		sqxc_pop_nested(xc);
	free(xc->buf);
	free(xc);
}

Sqxc* sqxc_new_chain(int io, ...)
{
	va_list    arg_list;
	SqxcInfo*  info;
	Sqxc*      xc = NULL;
	Sqxc*      cur;

	va_start(arg_list, io);
	io &= 1;    // 0 or 1
	for (;;) {
		info = va_arg(arg_list, SqxcInfo*);
		if (info == NULL)
			break;
		// create Sqxc chain
		if (xc == NULL)
			cur = xc = sqxc_new(info, io);
		else {
			cur->next = sqxc_new(info, io);
			cur->next->prev = cur;
			cur = cur->next;
		}
	};
	va_end(arg_list);

	// set Sqxc.dest
	xc->dest = cur;
	for (cur = xc->next;  cur;  cur = cur->next) {
//		cur->src = xc;
		cur->dest = xc->dest;
	}
	return xc;
}

void  sqxc_free_chain(Sqxc* xc)
{
	Sqxc*  next;

	// first Sqxc
	for (;  xc->prev;  xc = xc->prev)
		;
	// free Sqxc from first to last.
	for (;  xc;  xc = next) {
		next = xc->next;
		sqxc_free(xc);
	}
}

Sqxc*   sqxc_get(Sqxc* xc, const SqxcInfo* info, int nth)
{
	Sqxc* cur;
//	Sqxc* prev = NULL;
	int   count;

	for (count = 0, cur = xc;  cur;  cur = cur->next, count++) {
		// find sqxc by info
		if (info) {
			if (info == cur->info)
				return cur;
		}
		// get nth sqxc
		else if (count == nth)
			return cur;
//		prev = cur;
	}
	return NULL;
}

Sqxc*   sqxc_insert(Sqxc* xc, int position, Sqxc* xcdata)
{
	Sqxc* cur;
	Sqxc* prev = NULL;
	int   count;

	for (count = 0, cur = xc;  cur;  cur = cur->next, count++) {
		if (count == position)
			return cur;
		prev = cur;
	}
	if (prev)
		prev->next = xcdata;
	if (cur)
		cur->prev = xcdata;
	if (xc)
		xcdata->dest = xc->dest;
	xcdata->prev = prev;
	xcdata->next = cur;

	return (prev) ? xc : xcdata;
}

int   sqxc_broadcast(Sqxc* xc, int id, void* data)
{
	int   code = SQCODE_OK;

	// first Sqxc
	for (;  xc->prev;  xc = xc->prev)
		;
	// call Sqxc.notify() from first(src) to last(dest).
	for (;  xc;  xc = xc->next) {
		if (xc->ctrl) {
			xc->code = xc->ctrl(xc, id, data);
			if (xc->code != SQCODE_OK)
				code = xc->code;
		}
	}

	return code;
}

int  sqxc_send(Sqxc* src)
{
	Sqxc*    cur;
	Sqxc*    dest;
	SqxcType require_type;

	dest = src->dest;
	src->code = dest->send(dest, src);
	if (src->code == SQCODE_OK && dest->nested_count == 0)
		src->dest = dest->dest;
	// try to use other Sqxc if type not match
	else if (src->code == SQCODE_TYPE_NOT_MATCH) {
		require_type = dest->type;
		for (cur = dest;  cur != src;  cur = cur->prev) {
			if ((cur->supported_type & require_type) == 0)
				continue;
			// try to send data to previous Sqxc
			cur->dest = src->dest;
			src->dest = cur;
			if (cur->send(cur, src) == SQCODE_OK) {
				if(cur->nested_count == 0)
					src->dest = cur->dest;
				return (src->code = SQCODE_OK);
			}
		}
		// No Sqxc support required type
		src->code = SQCODE_TYPE_NOT_SUPPORT;
	}

	return src->code;
}

// ----------------------------------------------------------------------------
// push/pop nested object/array

#define NESTED_CHUNK_SIZE    (1 << 3)    // 8
#define NESTED_CHUNK_MASK    (NESTED_CHUNK_SIZE -1)

SqxcNested* sqxc_push_nested(Sqxc* xc)
{
	SqxcNested* outer = xc->nested;
	SqxcNested* nested;

/*	// every outer has one inner at the same time
	if (outer != NESTED_OUTER_ROOT && outer->inner) {
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
	if (outer != NESTED_OUTER_ROOT)
		outer->inner = nested;
	nested->outer = outer;
	nested->inner = NULL;
	xc->nested = nested;
	return nested;
}

void sqxc_pop_nested(Sqxc* xc)
{
	SqxcNested* outer;

	if (xc->nested != NESTED_OUTER_ROOT) {
		outer = xc->nested->outer;
		if (outer != NESTED_OUTER_ROOT)
			outer->inner = NULL;
		// free multiple SqxcNested each time
		xc->nested_count--;
		if ((xc->nested_count & NESTED_CHUNK_MASK) == 0)
			free(xc->nested);
		xc->nested = outer;
	}
}
