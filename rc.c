#include "rc.h"

#include "log.h"

#include <limits.h>
#include <stdlib.h>

struct rc {
    dealloc_fn *dealloc;
    unsigned count;
};

static inline size_t pad_to_pointer(size_t size) {
    size_t pointer_size = sizeof(void *);
    return ((size + pointer_size - 1) / pointer_size) * pointer_size;
}

static inline size_t padded_rc_size() {
    return pad_to_pointer(sizeof(struct rc));
}

void *rc_alloc(size_t size, dealloc_fn *dealloc) {
    const size_t data_size = padded_rc_size();
    void *result = NULL;
    if (size <= SIZE_MAX - data_size) {
        size += data_size;
        result = malloc(size);
        return_null_value(result, NULL);
        struct rc *r = result;
        r->count = 1;
        r->dealloc = dealloc;
        result += data_size;
    } else {
        log_error("can't alloc %lu bytes (too big)", size);
    }
    return result;
}

typedef void (wrapped_fn)(void *rc, struct rc *r);

static inline void _assert(void *rc, wrapped_fn wrapped) {
    return_null(rc);
    
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
}

static void _rc_retain(void *rc, struct rc *r) {
    ++r->count;
}

void rc_retain(void *rc) {
    _assert(rc, _rc_retain);
}

static inline void _rc_release(void *rc, struct rc *r) {
    if (!--r->count) {
        if (r->dealloc) { r->dealloc(rc); }
        free(r);
    }
}

void rc_release(void *rc) {
    _assert(rc, _rc_release);
}
