#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <malloc.h>

/* @breif 最小堆实现 ref libevent
 */

/*@ brief 自定义结构
 */
typedef struct my_struct_t {
	int data;	
	int min_heap_idx;
} my_struct_t;

/* @brief 最小堆结构
 */
typedef struct min_heap
{
	struct my_struct_t** p;  //数组
	unsigned n, a; //n-total a-current capacity
} min_heap_t;

inline void	     min_heap_ctor(min_heap_t* s);
inline void	     min_heap_dtor(min_heap_t* s);
inline void	     min_heap_elem_init(struct my_struct_t* e);
inline int	     min_heap_elt_is_top(const struct my_struct_t *e);
inline int	     min_heap_elem_greater(struct my_struct_t *a, struct my_struct_t *b);
inline int	     min_heap_empty(min_heap_t* s);
inline unsigned	     min_heap_size(min_heap_t* s);
inline struct my_struct_t*  min_heap_top(min_heap_t* s);
inline int	     min_heap_reserve(min_heap_t* s, unsigned n);
inline int	     min_heap_push(min_heap_t* s, struct my_struct_t* e);
inline struct my_struct_t*  min_heap_pop(min_heap_t* s);
inline int	     min_heap_erase(min_heap_t* s, struct my_struct_t* e);
inline void	     min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct my_struct_t* e);
inline void	     min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct my_struct_t* e);

int min_heap_elem_greater(struct my_struct_t *a, struct my_struct_t *b)
{
	return a->data > b->data;
}

void min_heap_ctor(min_heap_t* s) { s->p = 0; s->n = 0; s->a = 0; }
void min_heap_dtor(min_heap_t* s) { if (s->p) free(s->p); }
void min_heap_elem_init(struct my_struct_t* e) { e->min_heap_idx = -1; }
int min_heap_empty(min_heap_t* s) { return 0u == s->n; }
unsigned min_heap_size(min_heap_t* s) { return s->n; }
struct my_struct_t* min_heap_top(min_heap_t* s) { return s->n ? *s->p : 0; }

int min_heap_push(min_heap_t* s, struct my_struct_t* e)
{
	if (min_heap_reserve(s, s->n + 1))
		return -1;
	min_heap_shift_up_(s, s->n++, e);
	return 0;
}

struct my_struct_t* min_heap_pop(min_heap_t* s)
{
	if (s->n)
	{
		struct my_struct_t* e = *s->p;
		min_heap_shift_down_(s, 0u, s->p[--s->n]);
		e->min_heap_idx = -1;
		return e;
	}
	return 0;
}

int min_heap_elt_is_top(const struct my_struct_t *e)
{
	return e->min_heap_idx == 0;
}

int min_heap_erase(min_heap_t* s, struct my_struct_t* e)
{
	if (-1 != e->min_heap_idx)
	{
		struct my_struct_t *last = s->p[--s->n];
		unsigned parent = (e->min_heap_idx - 1) / 2;
		/* we replace e with the last element in the heap.  We might need to
		   shift it upward if it is less than its parent, or downward if it is
		   greater than one or both its children. Since the children are known
		   to be less than the parent, it can't need to shift both up and
		   down. */
		if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
			min_heap_shift_up_(s, e->min_heap_idx, last);
		else
			min_heap_shift_down_(s, e->min_heap_idx, last);
		e->min_heap_idx = -1;
		return 0;
	}
	return -1;
}

int min_heap_reserve(min_heap_t* s, unsigned n)
{
	if (s->a < n)
	{
		struct my_struct_t** p;
		unsigned a = s->a ? s->a * 2 : 8;
		if (a < n)
			a = n;
		if (!(p = (struct my_struct_t**)realloc(s->p, a * sizeof *p)))
			return -1;
		s->p = p;
		s->a = a;
	}
	return 0;
}

void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct my_struct_t* e)
{
	unsigned parent = (hole_index - 1) / 2;
	while (hole_index && min_heap_elem_greater(s->p[parent], e))
	{
		(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct my_struct_t* e)
{
	unsigned min_child = 2 * (hole_index + 1);
	while (min_child <= s->n)
	{
		min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
		if (!(min_heap_elem_greater(e, s->p[min_child])))
			break;
		(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}
	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

#endif
