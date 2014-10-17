#include "gc.h"

#include <stdlib.h>

#include "log.h"

static const int UNMARKED = 0;
static const int MARKED = 1;
static const int VISITED = 2;
static const int GLOBAL = 3;
static const int IN_CREATION = 4;

typedef struct gc_node {
	void *ptr;
	struct gc_node *next;
	int marked;
} gc_node;

static gc_node *all_nodes = NULL;
static gc_dispatcher *dispatchers = NULL;

gc_node *find_node(void *ptr) {
	return_unless(NULL, ptr);
	for (gc_node *cur = all_nodes; cur; cur = cur->next) {
		if (cur->ptr == ptr) return cur;
	}
	return NULL;
}

static bool mark_decendents(void *ptr) {
    return_unless(false, ptr);
    
    bool something_changed = false;
    for (gc_dispatcher *dispatcher = dispatchers; dispatcher; dispatcher = dispatcher->next) {
        if (dispatcher->is_matching(ptr)) {
            int count = dispatcher->number_of_references(ptr);
            for (int i = 0; i < count; ++i) {
                void *ref = dispatcher->reference_number(ptr, i);
                if (ref) {
                    gc_node *ref_node = find_node(ref);
                    if (ref_node && ref_node->marked == UNMARKED) {
                        ref_node->marked = MARKED;
                        something_changed = true;
                    }
                }
            }
            break;
        }
    }
    return something_changed;
}

void gc_collect() {
	for (gc_node *cur = all_nodes; cur; cur = cur->next) {
		if (cur->marked == VISITED || cur->marked == MARKED) {
			cur->marked = UNMARKED;
		}
	}

	bool something_changed = false;

    for (gc_node *cur = all_nodes; cur; cur= cur->next) {
        if (cur->marked == GLOBAL) {
            if (mark_decendents(cur->ptr)) something_changed = true;
        }
    }
    
	while (something_changed) {
		something_changed = false;
		for (gc_node *cur = all_nodes; cur; cur= cur->next) {
			if (cur->marked == MARKED) {
				cur->marked = VISITED;
                if (mark_decendents(cur->ptr)) something_changed = true;
			}
		}
	}

	for (gc_node *prev = NULL, *cur = all_nodes; cur;) {
		if (cur->marked == UNMARKED) {
			if (prev) {
				prev->next = cur->next;
			} else {
				all_nodes = cur->next;
			}
			gc_node *next = cur->next;
			free(cur);
			cur = next;
		} else {
			prev = cur;
			cur = cur->next;
		}
	}
}

static bool has_dispatcher(gc_dispatcher *dispatcher) {
	for (gc_dispatcher *current = dispatchers; current; current = current->next) {
		if (current == dispatcher) return true;
	}
	return false;
}

void gc_register_dispatcher(gc_dispatcher *dispatcher) {
	return_unless(, dispatcher, "no dispatcher");
	return_unless(, !dispatcher->next, "link set");
	return_unless(, !has_dispatcher(dispatcher), "double dispatcher");

	dispatcher->next = dispatchers;
	dispatchers = dispatcher;
}

void *gc_new(size_t size) {
	void *ptr = malloc(size);
	gc_node *node = malloc(sizeof(gc_node));

	if (!ptr || !node) {
		if (ptr) free(ptr);
		if (node) free(node);
		return_unless(NULL, false, "not enough memory");
	}

	node->ptr = ptr;
	node->marked = IN_CREATION;
	node->next = all_nodes;
	all_nodes = node;

	return ptr;
}

void gc_constructed(void *ptr) {
	return_unless(, ptr);

	gc_node *node = find_node(ptr);
	if (node) {
		return_unless(, node->marked == IN_CREATION, "no longer constructed");
		node->marked = GLOBAL;
	}
}

bool gc_add_global(void *ptr) {
	return_unless(false, ptr, "global is NULL");

	gc_node *node = find_node(ptr);
	if (node) {
		switch (node->marked) {
			case UNMARKED:
			case MARKED:
				node->marked = GLOBAL;
				// fallthrough
			case GLOBAL:
			case IN_CREATION:
				return true;
			default:
				return_unless(false, false, "unknow state");
		}
	}
	return false;
}

void gc_remove_global(void *ptr) {
	return_unless(, ptr, "global is NULL");

	gc_node *node = find_node(ptr);
	if (node) {
		if (node->marked == GLOBAL) {
			node->marked = UNMARKED;
		}
	}
}
