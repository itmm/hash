#include "t_rc.h"

#include "log.h"
#include "rc.h"

#pragma mark - track deallocs

static int _dealloc_count;

void _dealloc_counter(void *rc) {
    ++_dealloc_count;
}

void _init_counter(unit_state *state) {
    _dealloc_count = 0;
}

#pragma mark - tests

static void *alloc_chunk() {
    void *result = rc_alloc(10, rc_type_unknown, _dealloc_counter);
    return_unless(NULL, result);
    return result;
}

static void _simple(unit_state *state) {
    void *a = alloc_chunk();
    rc_release(a);
    assert_eq(state, 1, _dealloc_count);
}

static void _retain_avoids_dealloc(unit_state *state) {
    void *a = alloc_chunk();
    rc_retain(a);
    rc_release(a);
    assert_eq(state, 0, _dealloc_count);
    rc_release(a);
}

static void _retain_needs_double_release(unit_state *state) {
    void *a = alloc_chunk();
    rc_retain(a);
    rc_release(a);
    rc_release(a);
    assert_eq(state, 1, _dealloc_count);
}

static void _retain_NULL(unit_state *state) {
    rc_retain(NULL);
    assert_eq(state, 0, _dealloc_count);
}

static void _release_NULL(unit_state *state) {
    rc_release(NULL);
    assert_eq(state, 0, _dealloc_count);
}

static void _hash_NULL(unit_state *state) {
    assert_eq(state, 0, rc_hash(NULL));
}


#pragma mark - suite

unit_test *create_rc_tests() {
    return test_suite_alloc(6, (unit_test *[]) {
        test_alloc_with_wrappers(_init_counter, _simple, NULL),
        test_alloc_with_wrappers(_init_counter, _retain_avoids_dealloc, NULL),
        test_alloc_with_wrappers(_init_counter, _retain_needs_double_release, NULL),
        test_alloc_with_wrappers(_init_counter, _retain_NULL, NULL),
        test_alloc_with_wrappers(_init_counter, _release_NULL, NULL),
        test_alloc(_hash_NULL)
    });
}
