#include "t_rcstr.h"

#include "rc.h"
#include "rcstr.h"


#pragma mark - tests

static void _empty_string(unit_state *state) {
    rcstr rc = rcstr_dup("");
    assert_eq_str(state, "", rc);
    rc_release((void *) rc);
}

static void _some_string(unit_state *state) {
    rcstr rc = rcstr_dup("abc");
    assert_eq_str(state, "abc", rc);
    rc_release((void *) rc);
}

#pragma mark - suite

unit_test *create_rcstr_tests() {
    return test_suite_alloc("rcstr tests",
        test_alloc("empty string", _empty_string),
        test_alloc("some string", _some_string),
        NULL
    );
}
