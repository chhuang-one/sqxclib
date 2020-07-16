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

Sqxc*  sqxc_new(const SqxcInfo* cxinfo, int io)
{
	Sqxc* cx;
	SqInitFunc init;

	io &= 1;    // 0 or 1
	cx = (Sqxc*)calloc(1, cxinfo[io].size);
	cx->nested = NESTED_OUTER_ROOT;
	init = cxinfo[io].init;
	if (init)
		init(cx);
	cx->info = cxinfo;
	cx->io_ = io;
	return cx;
}

void  sqxc_free(Sqxc* cx)
{
	SqFinalFunc final;

	final = cx->info[cx->io_].final;
	if (final)
		final(cx);
	// free memory
	while (cx->nested != NESTED_OUTER_ROOT)
		sqxc_pop_nested(cx);
	free(cx->buf);
	free(cx);
}

Sqxc* sqxc_new_chain(int io, ...)
{
	va_list    arg_list;
	SqxcInfo*  info;
	Sqxc*      cx = NULL;
	Sqxc*      cur;

	va_start(arg_list, io);
	io &= 1;    // 0 or 1
	for (;;) {
		info = va_arg(arg_list, SqxcInfo*);
		if (info == NULL)
			break;
		// create Sqxc chain
		if (cx == NULL)
			cur = cx = sqxc_new(info, io);
		else {
			cur->next = sqxc_new(info, io);
			cur->next->prev = cur;
			cur = cur->next;
		}
	};
	va_end(arg_list);

	// set Sqxc.dest
	cx->dest = cur;
	for (cur = cx->next;  cur;  cur = cur->next) {
//		cur->src = cx;
		cur->dest = cx->dest;
	}
	return cx;
}

void  sqxc_free_chain(Sqxc* cx)
{
	Sqxc*  next;

	// first Sqxc
	for (;  cx->prev;  cx = cx->prev)
		;
	// free Sqxc from first to last.
	for (;  cx;  cx = next) {
		next = cx->next;
		sqxc_free(cx);
	}
}

Sqxc*   sqxc_get(Sqxc* cx, const SqxcInfo* info, int nth)
{
	Sqxc* cur;
//	Sqxc* prev = NULL;
	int   count;

	for (count = 0, cur = cx;  cur;  cur = cur->next, count++) {
		// find sqcx by info
		if (info) {
			if (info == cur->info)
				return cur;
		}
		// get nth sqcx
		else if (count == nth)
			return cur;
//		prev = cur;
	}
	return NULL;
}

Sqxc*   sqxc_insert(Sqxc* cx, int position, Sqxc* cxdata)
{
	Sqxc* cur;
	Sqxc* prev = NULL;
	int   count;

	for (count = 0, cur = cx;  cur;  cur = cur->next, count++) {
		if (count == position)
			return cur;
		prev = cur;
	}
	if (prev)
		prev->next = cxdata;
	if (cur)
		cur->prev = cxdata;
	if (cx)
		cxdata->dest = cx->dest;
	cxdata->prev = prev;
	cxdata->next = cur;

	return (prev) ? cx : cxdata;
}

int   sqxc_broadcast(Sqxc* cx, int id, void* data)
{
	int   code = SQCODE_OK;

	// first Sqxc
	for (;  cx->prev;  cx = cx->prev)
		;
	// call Sqxc.notify() from first(src) to last(dest).
	for (;  cx;  cx = cx->next) {
		if (cx->ctrl) {
			cx->code = cx->ctrl(cx, id, data);
			if (cx->code != SQCODE_OK)
				code = cx->code;
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

SqxcNested* sqxc_push_nested(Sqxc* cx)
{
	SqxcNested* outer = cx->nested;
	SqxcNested* nested;

/*	// every outer has one inner at the same time
	if (outer != NESTED_OUTER_ROOT && outer->inner) {
		cx->code = SQCODE_TOO_MANY_NESTED;
		return NULL;
	}
 */

	// alloc multiple SqxcNested each time
	if ((cx->nested_count & NESTED_CHUNK_MASK) == 0)
		nested = (SqxcNested*)malloc(sizeof(SqxcNested) * NESTED_CHUNK_SIZE);
	else
		nested = cx->nested +1;
	cx->nested_count++;

	// link
	if (outer != NESTED_OUTER_ROOT)
		outer->inner = nested;
	nested->outer = outer;
	nested->inner = NULL;
	cx->nested = nested;
	return nested;
}

void sqxc_pop_nested(Sqxc* cx)
{
	SqxcNested* outer;

	if (cx->nested != NESTED_OUTER_ROOT) {
		outer = cx->nested->outer;
		if (outer != NESTED_OUTER_ROOT)
			outer->inner = NULL;
		// free multiple SqxcNested each time
		cx->nested_count--;
		if ((cx->nested_count & NESTED_CHUNK_MASK) == 0)
			free(cx->nested);
		cx->nested = outer;
	}
}
