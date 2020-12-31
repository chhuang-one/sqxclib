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

#ifndef SQ_TYPE_STL_CPP_H
#define SQ_TYPE_STL_CPP_H

#include <type_traits>    // std::is_pointer

#include <SqError.h>
#include <SqType.h>
#include <SqxcValue.h>

#ifdef __cplusplus

namespace Sq {

// SqType for C++ STL containers
template<class Container>
struct TypeStl : SqType {
	static void  cxxInit(void* instance, const SqType* type) {
		new (&(*(Container*)instance)) Container();
	}
	static void  cxxFinal(void* instance, const SqType* type) {
		((Container*)instance)->~Container();
	}

	static int   cxxParse(void* instance, const SqType* type, Sqxc* src) {
		SqType*     element_type = (SqType*)type->entry;    // TypeStl use SqType.entry to store element type
		SqxcValue*  xc_value = (SqxcValue*)src->dest;
		SqxcNested* nested;
		void*       element;

		// Start of Array - Frist time to call this function to parse array
		nested = xc_value->nested;
		if (nested->data != instance) {
			if (src->type != SQXC_TYPE_ARRAY) {
//				src->required_type = SQXC_TYPE_ARRAY;    // set required type if return SQCODE_TYPE_NOT_MATCH
				return (src->code = SQCODE_TYPE_NOT_MATCH);
			}
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data = instance;
			nested->data2 = (void*)type;
			return (src->code = SQCODE_OK);
		}
		/*
		// End of Array : sqxc_value_send() have done it.
		else if (src->type == SQXC_TYPE_ARRAY_END) {
			sqxc_pop_nested(src);
			return (src->code = SQCODE_OK);
		}
		*/

		((Container*)instance)->emplace_back(typename Container::value_type());
		element = (void*) std::addressof(((Container*)instance)->back());
		element = sq_type_init_instance(element_type, element,
				std::is_pointer<typename Container::value_type>::value);
		src->name = NULL;    // set "name" before calling parse()
		src->code = element_type->parse(element, element_type, src);
		return src->code;
	}

	static Sqxc* cxxWrite(void* instance, const SqType* type, Sqxc* dest) {
		SqType*     element_type = (SqType*)type->entry;    // TypeStl use SqType.entry to store element type
		const char* array_name = dest->name;
		void*       element;
		Container* container = (Container*)instance;
		typename Container::iterator cur, end;

		// Begin of SQXC_TYPE_ARRAY
		dest->type = SQXC_TYPE_ARRAY;
//		dest->name = array_name;    // "name" was set by caller of this function
//		dest->value.pointer = NULL;
		dest = sqxc_send(dest);
		if (dest->code != SQCODE_OK)
			return dest;

		// output elements
		for (cur = container->begin(), end = container->end(); cur != end;  cur++) {
			element = (void*) &*cur;
			if (std::is_pointer<typename Container::value_type>::value)
				element = *(void**)element;
			dest = element_type->write(element, element_type, dest);
			if (dest->code != SQCODE_OK)
				return dest;
		}

		// End of SQXC_TYPE_ARRAY
		dest->type = SQXC_TYPE_ARRAY_END;
		dest->name = array_name;
		dest = sqxc_send(dest);
		return dest;
	}

	TypeStl(SqType* element_type) {
		this->size  = sizeof(Container);
		this->init  = cxxInit;
		this->final = cxxFinal;
		this->parse = cxxParse;
		this->write = cxxWrite;
		this->entry = (SqEntry**)element_type;    // TypeStl use SqType.entry to store element type
		this->n_entry = -1;                       // SqType.entry can't be freed if SqType.n_entry == -1
	}

	// for dynamic allocated Sq::TypeStl
	void* operator new(size_t size) {
		void* memory = malloc(size);
		((TypeStl*)memory)->bit_field = SQB_DYNAMIC;
		return memory;
	}
	void operator delete(void* instance) {
		free(instance);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TYPE_STL_CPP_H
