#include "t_rc.h"

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

static void _simple(unit_state *state) {
    void *a = rc_alloc(10, _dealloc_counter);
    rc_release(a);
    assert_eq(state, 1, _dealloc_count);
}

static void _retain_avoids_dealloc(unit_state *state) {
    void *a = rc_alloc(10, _dealloc_counter);
    rc_retain(a);
    rc_release(a);
    assert_eq(state, 0, _dealloc_count);
    rc_release(a);
}

static void _retain_needs_double_release(unit_state *state) {
    void *a = rc_alloc(10, _dealloc_counter);
    rc_retain(a);
    rc_release(a);
    rc_release(a);
    assert_eq(state, 1, _dealloc_count);
}

#pragma mark - suite

unit_test *create_rc_tests() {
    return test_suite_alloc("rcstr tests",
        test_full_alloc("simple", _init_counter, _simple, NULL),
        test_full_alloc("simple", _init_counter, _retain_avoids_dealloc, NULL),
        test_full_alloc("simple", _init_counter, _retain_needs_double_release, NULL),
        NULL
    );
}
