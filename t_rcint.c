#include "t_rcint.h"

#include "log.h"
#include "rc.h"
#include "rcint.h"
#include "rcstr.h"

#include <stdio.h>
#include <limits.h>

#pragma mark - tests

static void _simple(unit_state *state) {
    rcint *a = rcint_alloc(42);
    assert_eq(state, 42, rcint_value(a));
    rc_release(a);
}

static void _test_str(unit_state *state, int expected) {
    rcint *a = rcint_alloc(expected);
    return_unless(, a);
    
    rcstr *got = rc2str(a);
    return_unless(, got);
    rc_release(a);
    
    static char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", expected);
    
    assert_eq_str(state, buffer, rcstr_str(got));
    rc_release(got);
}

static void _zero_str(unit_state *state) {
    _test_str(state, 0);
}

static void _max_str(unit_state *state) {
    _test_str(state, INT_MAX);
}

static void _min_str(unit_state *state) {
    _test_str(state, INT_MIN);
}

static void _hash_pos(unit_state *state) {
    rcint *a = rcint_alloc(42);
    assert_eq(state, 42, rc_hash(a));
}

static void _hash_neg(unit_state *state) {
    rcint *a = rcint_alloc(-42);
    assert_eq(state, -42 & INT_MAX, rc_hash(a));
}

#pragma mark - suite

unit_test *create_rcint_tests() {
    return test_suite_alloc(6, (unit_test *[]) {
        test_alloc(_simple),
        test_alloc(_zero_str),
        test_alloc(_max_str),
        test_alloc(_min_str),
        test_alloc(_hash_pos),
        test_alloc(_hash_neg)
    });
}
