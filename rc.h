#if !defined(rc_h)
#define rc_h

    #include <stdlib.h>

    typedef void (dealloc_fn)(void *rc);

    void *rc_alloc(size_t size, dealloc_fn *dealloc);
    void rc_retain(void *rc);
    void rc_release(void *rc);

#endif
