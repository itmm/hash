#include "t_rcstr.h"

#include "rc.h"
#include "rcstr.h"


#pragma mark - tests

static inline void _test_string(unit_state *state, const char *str) {
    rcstr rc = rcstr_dup(str);
    assert_eq_str(state, str, rc);
    rc_release(rc);
}

static void _empty_string(unit_state *state) {
    _test_string(state, "");
}

static void _some_string(unit_state *state) {
    _test_string(state, "abc");
}

#pragma mark - suite

unit_test *create_rcstr_tests() {
    return test_suite_alloc(2, (unit_test *[]) {
        test_alloc(_empty_string),
        test_alloc(_some_string)
    });
}
