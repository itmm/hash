#include "rc.h"

#include "log.h"

#include <limits.h>
#include <stdlib.h>

struct rc {
    dealloc_fn *dealloc;
    unsigned count;
    rc_type type;
};

static inline size_t pad_to_pointer(size_t size) {
    size_t pointer_size = sizeof(void *);
    return ((size + pointer_size - 1) / pointer_size) * pointer_size;
}

static inline size_t padded_rc_size() {
    return pad_to_pointer(sizeof(struct rc));
}

void *rc_alloc(size_t size, rc_type type, dealloc_fn *dealloc) {
    const size_t data_size = padded_rc_size();
    void *result = NULL;
    if (size <= SIZE_MAX - data_size) {
        size += data_size;
        result = malloc(size);
        return_value_unless(result, NULL);
        struct rc *r = result;
        r->dealloc = dealloc;
        r->count = 1;
        r->type = type;
        result += data_size;
    } else {
        log_error("can't alloc %lu bytes (too big)", size);
    }
    return result;
}

typedef void (wrapped_fn)(void *rc, struct rc *r);

static inline void *_assert(void *rc, wrapped_fn wrapped) {
    if (!rc) { return NULL; }
    
    struct rc *r = rc - padded_rc_size();
    if (!r->count) {
        log_error("object %p processed after dealloc", rc);
    }
    else if (r->count >= UINT_MAX) {
        log_error("object %p retained too many times (will leak)", rc);
    }
    else {
        wrapped(rc, r);
    }
    
    return rc;
}

static void _rc_retain(void *rc, struct rc *r) {
    ++r->count;
}

void *rc_retain(void *rc) {
    return _assert(rc, _rc_retain);
}

static inline void _rc_release(void *rc, struct rc *r) {
    if (!--r->count) {
        if (r->dealloc) { r->dealloc(rc); }
        free(r);
    }
}

void *rc_release(void *rc) {
    return _assert(rc, _rc_release);
}

rc_type rc_get_type(void *rc) {
    return_value_unless(rc, rc_type_unknown);
    struct rc *r = rc - padded_rc_size();
    return r->type;
}
