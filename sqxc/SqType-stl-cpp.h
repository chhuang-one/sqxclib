/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#ifndef SQ_TYPE_STL_CPP_H
#define SQ_TYPE_STL_CPP_H

#ifdef __cplusplus

#include <type_traits>    // std::is_pointer, std::is_reference

#include <SqConfig.h>
#include <SqError.h>
#include <SqType.h>
#include <SqxcValue.h>

#define SQ_TYPE_STL_ENABLE_DYNAMIC     0

namespace Sq {

// SqType for C++ STL containers
template<class Container>
struct TypeStl : SqType {
	static void  cxxInit(void *instance, const SqType *type) {
		new (&(*(Container*)instance)) Container();
	}
	static void  cxxFinal(void *instance, const SqType *type) {
		((Container*)instance)->~Container();
	}

	static int   cxxParse(void *instance, const SqType *type, Sqxc *src) {
		SqType     *element_type = (SqType*)type->entry;    // TypeStl use SqType.entry to store element type
		SqxcValue  *xc_value = (SqxcValue*)src->dest;
		SqxcNested *nested;
		void       *element;

		// Start of Array (Container)
		nested = xc_value->nested;
#if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
		if (nested->data3 != instance) {
			if (nested->data != instance) {
				// First time to call this function to parse array (container)
				nested = sqxc_push_nested((Sqxc*)xc_value);
				nested->data = instance;
				nested->data2 = (void*)type;
				nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
			}
			if (src->type != SQXC_TYPE_ARRAY) {
				/* set required type if return SQCODE_TYPE_NOT_MATCHED
				src->required_type = SQXC_TYPE_ARRAY;
				*/
				return (src->code = SQCODE_TYPE_NOT_MATCHED);
			}
			// SqxcNested is ready to parse array (container), type has been matched.
			nested->data3 = instance;
			return (src->code = SQCODE_OK);
		}
#else
	if (nested->data != instance) {
		// do type match
		if (src->type != SQXC_TYPE_ARRAY) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_ARRAY;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = instance;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
#endif  // SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
		/*
		// End of Array (Container) : sqxc_value_send() have done it.
		else if (src->type == SQXC_TYPE_ARRAY_END) {
			sqxc_pop_nested(src);
			return (src->code = SQCODE_OK);
		}
		 */

		((Container*)instance)->emplace_back(typename Container::value_type());
		element = (void*) std::addressof(((Container*)instance)->back());
		element = sq_type_init_instance(element_type, element,
				std::is_pointer<typename Container::value_type>::value ||
				std::is_reference<typename Container::value_type>::value);
		src->name = NULL;    // set "name" before calling parse()
		src->code = element_type->parse(element, element_type, src);
		return src->code;
	}

	static Sqxc *cxxWrite(void *instance, const SqType *type, Sqxc *dest) {
		SqType     *element_type = (SqType*)type->entry;    // TypeStl use SqType.entry to store element type
		const char *array_name = dest->name;
		void       *element;
		Container  *container = (Container*)instance;
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
			if (std::is_pointer<typename Container::value_type>::value || std::is_reference<typename Container::value_type>::value)
				element = *(void**)element;
			dest = element_type->write(element, element_type, dest);
			if (dest->code != SQCODE_OK)
				return dest;
		}

		// End of SQXC_TYPE_ARRAY
		dest->type = SQXC_TYPE_ARRAY_END;
		dest->name = array_name;
//		dest->value.pointer = NULL;
		dest = sqxc_send(dest);
		return dest;
	}

	TypeStl(const SqType *element_type) {
		this->size  = sizeof(Container);
		this->init  = cxxInit;
		this->final = cxxFinal;
		this->parse = cxxParse;
		this->write = cxxWrite;
		this->name  = NULL;
		this->n_entry = -1;                       // SqType.entry isn't freed if SqType.n_entry == -1
		this->entry = (SqEntry**)element_type;    // TypeStl use SqType.entry to store element type
#if SQ_TYPE_STL_ENABLE_DYNAMIC == 1
		// reset SqType::bit_field if it has not been set by operator new()
		if (this->bit_field != SQB_TYPE_DYNAMIC)
			this->bit_field = 0;
#else
		this->bit_field = 0;
#endif
	}

#if SQ_TYPE_STL_ENABLE_DYNAMIC == 1
	~TypeStl() {
		/*
		// The destructor of the base class was originally called here.
		// But it just doesn't need to call this.
		if (this->bit_field & SQB_TYPE_DYNAMIC)
			sq_type_final_self(this);
		 */
	}

	// for dynamic allocated Sq::TypeStl
	void *operator new(size_t size) {
		void *instance = malloc(size);
		((TypeStl*)instance)->bit_field = SQB_TYPE_DYNAMIC;
		return instance;
	}
	void operator delete(void *instance) {
		free(instance);
	}
#endif  // SQ_TYPE_STL_ENABLE_DYNAMIC
};

};  // namespace Sq

#endif  // __cplusplus

#undef SQ_TYPE_STL_ENABLE_DYNAMIC

#endif  // SQ_TYPE_STL_CPP_H
