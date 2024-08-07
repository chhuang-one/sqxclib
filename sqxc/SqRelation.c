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

#include <stdlib.h>

#include <SqRelation.h>

typedef struct SqRHeader         SqRHeader;

// ----------------------------------------------------------------------------
// SqRHeader: header of chunk in pool

// size of SqRHeader == size of 2 pointers
struct SqRHeader {
	SqRHeader  *next;
	uintptr_t   n_allocated;    // number of allocated node in current chunk
};

// ----------------------------------------------------------------------------
// SqRelationPool: collection of relation node

struct SqRelationPool {
	SqRHeader       *head;      // pointer to first chunk's header (SqRHeader)
	SqRHeader       *tail;      // pointer to last chunk's header (SqRHeader)

	SqRelationNode  *freed;     // pointer to last freed node
	unsigned int     n_freed;   // number of all freed node
	unsigned int     n_used;    // number of all used node
	unsigned int     chunk_size;
};

//     int cmp_object__node_object(void *object, SqRelationNode *node);
static int cmp_object__node_object(const void *object, const void *node);


SqRelationPool *sq_relation_pool_create(unsigned int chunk_size) {
	SqRelationPool *rpool;

	rpool = malloc(sizeof(SqRelationPool));
	rpool->head = malloc(sizeof(SqRHeader) + sizeof(SqRelationNode) * chunk_size);
	rpool->head->next = NULL;
	rpool->head->n_allocated = 0;
	rpool->tail = rpool->head;
	rpool->freed = NULL;
	rpool->n_freed = 0;
	rpool->n_used = 0;
	rpool->chunk_size = chunk_size;
	return rpool;
}

void  sq_relation_pool_destroy(SqRelationPool *rpool) {
	SqRHeader *cur, *next;

	for (cur = rpool->head;  cur;  cur = next) {
		next = cur->next;
		free(cur);
	}
	free(rpool);
}

SqRelationNode *sq_relation_pool_alloc(SqRelationPool *rpool) {
	SqRelationNode *rnode;

	if (rpool->freed) {
		rnode = rpool->freed;
		rpool->freed = rnode->next;
		rpool->n_freed--;
	}
	else if (rpool->tail->n_allocated < rpool->chunk_size) {
		rnode = (SqRelationNode*) ((char*)rpool->tail + sizeof(SqRHeader));
		rnode += rpool->tail->n_allocated;
		rpool->tail->n_allocated++;
	}
	else {
		rpool->tail->next = malloc(sizeof(SqRHeader) + sizeof(SqRelationNode) * rpool->chunk_size);
		rpool->tail = rpool->tail->next;
		rpool->tail->next = NULL;
		rpool->tail->n_allocated = 1;    // allocate first one in chunk
		rnode = (SqRelationNode*) ((char*)rpool->tail + sizeof(SqRHeader));
	}
	rpool->n_used++;
//	rnode->next = NULL;
	return rnode;
}

void  sq_relation_pool_free(SqRelationPool *rpool, SqRelationNode *rnode) {
	rnode->next = rpool->freed;
	rpool->freed = rnode;
	rpool->n_freed++;
	rpool->n_used--;
}

// ----------------------------------------------------------------------------
// SqRelationNode

SqRelationNode *sq_relation_node_find(SqRelationNode *node, const void *object, SqRelationNode **prev_of_returned_node) {
	SqRelationNode *prev = NULL;

	for (;  node;  node = node->next) {
		if (node->object == object) {
			if (prev_of_returned_node)
				*prev_of_returned_node = prev;
			return node;
		}
		prev = node;
	}
	return NULL;
}

SqRelationNode *sq_relation_node_reverse(SqRelationNode *node) {
	SqRelationNode *node_next, *node_prev;

	// reverse order
	for (node_prev = NULL;  node;  node = node_next) {
		node_next = node->next;
		node->next = node_prev;
		node_prev = node;
	}
	return node_prev;
}

// ----------------------------------------------------------------------------
// SqRelation

SqRelation *sq_relation_init(SqRelation *relation, SqRelationPool *rpool, unsigned int capacity) {
	sq_array_init(relation, sizeof(SqRelationNode), capacity);
	relation->pool = rpool;
	return relation;
}

SqRelation *sq_relation_final(SqRelation *relation) {
	sq_array_final(relation);
	return relation;
}

void  sq_relation_clear(SqRelation *relation) {
	SqRelationNode *node, *node_end, *node_pool, *node_next;

	node_end = sq_array_end(relation, SqRelationNode);
	for (node = relation->data;  node < node_end;  node++) {
		for (node_pool = node->next;  node_pool;  node_pool = node_next) {
			node_next = node_pool->next;
			sq_relation_pool_free(relation->pool, node_pool);
		}
		node->next = NULL;
	}
}

void  sq_relation_add(SqRelation *relation, const void *from, const void *to, int no_reverse) {
	unsigned int    index;
	SqRelationNode *rnode_from, *rnode_to, *rnode_pool;

	rnode_from = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, from, cmp_object__node_object, &index);
	if (rnode_from == NULL) {
		rnode_from = sq_array_alloc_at(relation, index, 1);
		rnode_from->object = (void*)from;
		rnode_from->next = NULL;
	}

	if (to) {
		// add 'to' relation to 'from' object
		rnode_pool = sq_relation_pool_alloc(relation->pool);
		rnode_pool->object = (void*)to;
		rnode_pool->next = rnode_from->next;
		rnode_from->next = rnode_pool;
		// add reverse reference
		if (no_reverse == 0) {
			rnode_to = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, to, cmp_object__node_object, &index);
			if (rnode_to == NULL) {
				rnode_to = sq_array_alloc_at(relation, index, 1);
				rnode_to->object = (void*)to;
				rnode_to->next = NULL;
			}
			// add 'from' relation to 'to' object
			rnode_pool = sq_relation_pool_alloc(relation->pool);
			rnode_pool->object = (void*)from;
			rnode_pool->next = rnode_to->next;
			rnode_to->next = rnode_pool;
		}
	}
}

void  sq_relation_erase(SqRelation *relation, const void *from, const void *to, int no_reverse, SqDestroyFunc object_free_func) {
	SqRelationNode *rnode, *rnode_pool;

	rnode = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, from, cmp_object__node_object, NULL);
	if (rnode == NULL)
		return;

	for (rnode_pool = rnode->next;  rnode_pool;  rnode_pool = rnode->next) {
		if (rnode_pool->object == to || to == NULL) {
			if (no_reverse != 1) {
				sq_relation_erase(relation, rnode_pool->object, from, 1, NULL);
				if (no_reverse == -1)
					sq_relation_erase(relation, rnode_pool->object, NULL, 0, NULL);
			}
			rnode->next = rnode_pool->next;
			if (no_reverse == -1 && object_free_func)
				object_free_func(rnode_pool->object);
			sq_relation_pool_free(relation->pool, rnode_pool);
		}
		else
			rnode = rnode->next;
	}
}

void  sq_relation_replace(SqRelation *relation, const void *old_object, const void *new_object, int no_reverse) {
	SqRelationNode *rnode, *rnode_new, *rnode_pool;
	unsigned int  new_index;

	rnode = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, old_object, cmp_object__node_object, NULL);
	if (rnode == NULL)
		return;
	rnode_new = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, new_object, cmp_object__node_object, &new_index);
	if (rnode_new == NULL)
		rnode_new = sq_array_alloc_at(relation, new_index, 1);
	else if (rnode_new->next) {
		for (rnode_pool = rnode; rnode_pool->next; rnode_pool = rnode_pool->next)
			;
		rnode_pool->next = rnode_new->next;
	}
	rnode_new->next = rnode->next;
	rnode_new->object = (void*)new_object;
	rnode->next = NULL;
	rnode = rnode_new;

	if (no_reverse == 0) {
		for (rnode = rnode->next;  rnode;  rnode = rnode->next) {
			rnode_pool = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, rnode->object, cmp_object__node_object, NULL);
			if (rnode_pool == NULL)
				continue;
			for (rnode_pool = rnode_pool->next;  rnode_pool;  rnode_pool = rnode_pool->next) {
				if (rnode_pool->object == old_object)
					rnode_pool->object = (void*)new_object;
			}
		}
	}
}

void  sq_relation_remove_empty(SqRelation *relation) {
	SqRelationNode *rnode_src, *rnode_dest, *rnode_end;

	rnode_end = relation->data + relation->length;
	// find first SqRelationNode that has no reference
	for (rnode_dest = relation->data;  rnode_dest < rnode_end;  rnode_dest++) {
		if (rnode_dest->next == NULL)
			break;
	}
	// move SqRelationNode to overwrite empty SqRelationNode
	for (rnode_src = rnode_dest +1;  rnode_src < rnode_end;  rnode_src++) {
		if (rnode_src->next) {
			rnode_dest->next   = rnode_src->next;
			rnode_dest->object = rnode_src->object;
			rnode_dest++;
		}
	}

	relation->length = rnode_dest - relation->data;
}

SqRelationNode *sq_relation_find(SqRelation *relation, const void *from, const void *to) {
	SqRelationNode *rnode;

	rnode = SQ_ARRAY_FIND_SORTED(relation, SqRelationNode, from, cmp_object__node_object, NULL);
	if (rnode && to)
		return sq_relation_node_find(rnode->next, to, NULL);

	return rnode;
}

// ----------------------------------------------------------------------------
// static functions

static int cmp_object__node_object(const void *object, const void *node)
{
	return object - ((SqRelationNode*)node)->object;
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqRelation.h

#else   // __STDC_VERSION__
// define C/C++ functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__

