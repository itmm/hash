#if !defined(gc_h)
#define gc_h

	#include <stdbool.h>
    #include <stdlib.h>

	typedef struct gc_dispatcher {
		struct gc_dispatcher *next;
		bool (* is_matching)(void *ptr);
		int (* number_of_references)(void *ptr);
		void *(* reference_number)(void *ptr, int index);
	} gc_dispatcher;

	void gc_collect();

	void gc_register_dispatcher(gc_dispatcher *dispatcher);

	void *gc_new(size_t size);
	void gc_constructed(void *ptr);

	bool gc_add_global(void *ptr);
	void gc_remove_global(void *ptr);

#endif