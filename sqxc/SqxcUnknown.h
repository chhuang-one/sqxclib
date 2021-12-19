/*
 *   Copyright (C) 2021 by C.H. Huang
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

#ifndef SQXC_UNKNOWN_H
#define SQXC_UNKNOWN_H

#include <Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct Sqxc             SqxcUnknown;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo          *SQXC_INFO_UNKNOWN;

#define sqxc_unknown_new()      sqxc_new(SQXC_INFO_UNKNOWN)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
	SqxcUnknown - A Sqxc element for unknown data.

	Sqxc
	|
	`--- SqxcUnknown
 */

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct XcUnknown : SqxcUnknown
{
	XcUnknown() {
		sqxc_init((Sqxc*)this, SQXC_INFO_UNKNOWN);
	}
	~XcUnknown() {
		sqxc_final((Sqxc*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQXC_UNKNOWN_H
