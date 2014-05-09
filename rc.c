#include "rc.h"

#include "log.h"
#include "rcint.h"
#include "rclist.h"

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

typedef void *(wrapped_fn)(void *rc, struct rc *r);

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
        rc = wrapped(rc, r);
    }
    
    return rc;
}

static inline void *_rc_retain(void *rc, struct rc *r) {
    ++r->count;
    return rc;
}

void *rc_retain(void *rc) {
    return _assert(rc, _rc_retain);
}

static inline void *_rc_release(void *rc, struct rc *r) {
    if (!--r->count) {
        if (r->dealloc) { r->dealloc(rc); }
        free(r);
        return NULL;
    }
    return rc;
}

void *rc_release(void *rc) {
    return _assert(rc, _rc_release);
}

rc_type rc_get_type(void *rc) {
    return_value_unless(rc, rc_type_unknown);
    struct rc *r = rc - padded_rc_size();
    return r->type;
}

int rc_hash(void *rc) {
    if (!rc) { return 0; }
    switch (rc_get_type(rc)) {
        case rc_type_string: return rcstr_hash((rcstr *) rc);
        case rc_type_int: return rcint_hash((rcint *) rc);
        case rc_type_list: return rclist_hash((rclist *) rc);
        case rc_type_false: return 1;
        case rc_type_true: return 2;
        default:
            log_error("can't hash type %d", (int) rc_get_type(rc));
            return 0;
    }
}

void *rc_true() {
    static void *_singleton = NULL;
    if (!_singleton) {
        _singleton = rc_alloc(0, rc_type_true, NULL);
        return_value_unless(_singleton, NULL);
    }
    return rc_retain(_singleton);
}

void *rc_false() {
    static void *_singleton = NULL;
    if (!_singleton) {
        _singleton = rc_alloc(0, rc_type_false, NULL);
        return_value_unless(_singleton, NULL);
    }
    return rc_retain(_singleton);
}
