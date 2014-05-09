#if !defined(rc_h)
#define rc_h

    #include <stdlib.h>

    typedef enum {
        rc_type_unknown,
        rc_type_string,
        rc_type_list,
        rc_type_hash,
        rc_type_int,
        rc_type_true,
        rc_type_false
    } rc_type;

    typedef void (dealloc_fn)(void *rc);

    void *rc_alloc(size_t size, rc_type type, dealloc_fn *dealloc);
    void *rc_retain(void *rc);
    void *rc_release(void *rc);

    void *rc_true();
    void *rc_false();

    int rc_hash(void *rc);

    rc_type rc_get_type(void *rc);

#endif
