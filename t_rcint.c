#include "t_rcint.h"

#include "rc.h"
#include "rcint.h"

#pragma mark - tests

static void _simple(unit_state *state) {
    void *a = rcint_alloc(42);
    assert_eq(state, 42, *((int *) a));
    rc_release(a);
}

#pragma mark - suite

unit_test *create_rcint_tests() {
    return test_suite_alloc(1, (unit_test *[]) {
         test_alloc(_simple)
    });
}
