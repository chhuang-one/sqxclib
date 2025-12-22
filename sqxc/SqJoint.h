/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#ifndef SQ_JOINT_H
#define SQ_JOINT_H

#include <sqxc/SqTable.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqType         SqTypeJoint;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/*	SqTypeJoint is the default type for handling query that join multi-table.
	            It can create array of pointers for query result.

	SqType
	|
	`--- SqTypeJoint

	-- SQL statement
	SELECT 'T01'.'column' AS 'T01.column',
	       'T02'.'column' AS 'T02.column'
	FROM table1 AS T01
	JOIN table2 AS T02 ON T02.table1_id = T01.id
	WHERE T01.column > 100

	// --- C code ---
	sq_type_joint_add(type_joint, table1, "T01");
	sq_type_joint_add(type_joint, table2, "T02");

	-- SQL statement
	SELECT 'table1'.'column' AS 'table1.column',
	       'table2'.'column' AS 'table2.column'
	FROM table1
	JOIN table2 ON table2.table1_id = table1.id
	WHERE table1.column > 100

	// --- C code ---
	sq_type_joint_add(type_joint, table1, NULL);
	sq_type_joint_add(type_joint, table2, NULL);

	---------------------------------------------
	// SqxcValue will get below data structure:

	void **instance = sqxc_value_instance(xcvalue);

	table1 = instance[0];
	table2 = instance[1];
 */

SqTypeJoint *sq_type_joint_new();

// void sq_type_joint_free(SqTypeJoint *type_joint);
#define sq_type_joint_free(type_joint)     \
		sq_type_free((SqType*)(type_joint))

void    sq_type_joint_init(SqTypeJoint *type_joint);

// void sq_type_joint_final(SqTypeJoint *type_joint);
#define sq_type_joint_final(type_joint)    \
		sq_type_final_self((SqType*)(type_joint))

// derived struct can use these functions
void  sq_type_joint_add(SqTypeJoint *type_joint, SqTable *table, const char *table_as_name);
void  sq_type_joint_erase(SqTypeJoint *type_joint, SqTable *table, const char *table_as_name);
void  sq_type_joint_clear(SqTypeJoint *type_joint);

// alias of sq_type_joint_erase()
// void sq_type_joint_remove(SqTypeJoint *type_joint, SqTable *table, const char *table_as_name);
#define sq_type_joint_remove        sq_type_joint_erase

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	TypeJointMethod is used by SqTypeJoint's children.

	It's derived struct/class must be C++11 standard-layout and has SqTypeJoint members.
 */
struct TypeJointMethod
{
	void  add(SqTable *table, const char *table_as_name = NULL);
	void  erase(SqTable *table, const char *table_as_name = NULL);
	void  remove(SqTable *table, const char *table_as_name = NULL);
	void  clear();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqTypeJoint - combine multiple table's type (SQL joined table)

	SqType
	|
	`--- SqTypeJoint
 */
#define SQ_TYPE_JOINT_MEMBERS    SQ_TYPE_MEMBERS

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

inline void  TypeJointMethod::add(SqTable *table, const char *table_as_name) {
	sq_type_joint_add((SqTypeJoint*)this, table, table_as_name);
}
inline void  TypeJointMethod::erase(SqTable *table, const char *table_as_name) {
	sq_type_joint_erase((SqTypeJoint*)this, table, table_as_name);
}
inline void  TypeJointMethod::remove(SqTable *table, const char *table_as_name) {
	sq_type_joint_erase((SqTypeJoint*)this, table, table_as_name);
}
inline void  TypeJointMethod::clear() {
	sq_type_joint_clear((SqTypeJoint*)this);
}

/* All derived struct/class must be C++11 standard-layout. */

struct TypeJoint : TypeJointMethod,                // <-- 1. inherit C++ method
                   SqTypeJoint                     // <-- 2. inherit member variable
{
	// It doesn't need this line because current struct already inherits from SqTypeJoint.
//	SQ_TYPE_JOINT_MEMBERS;

	// constructor
	TypeJoint() {
		sq_type_joint_init((SqTypeJoint*)this);
	}
	// destructor
	~TypeJoint() {
		sq_type_joint_final((SqTypeJoint*)this);
	}
};

// This is for directly use only. You can NOT derived it.
template <unsigned int n_tables>
struct Joint
{
	void *data[n_tables];

	// operator
	void *&operator[](int index) {
		return data[index];
	}
	// const Joint will use this function.
	void  *operator[](int index) const {
		return data[index];
	}
};

};  // namespace Sq

#endif  // __cplusplus


// ----------------------------------------------------------------------------
// SqJoint : combine multiple table's instance (SQL joined table)

#ifdef __cplusplus
#define SQ_JOINT(n_tables)    Sq::Joint<n_tables>
#else
#define SQ_JOINT(n_tables)    struct { void *data[n_tables]; }
#endif


#endif  // SQ_JOINT_H

