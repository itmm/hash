#include "t_rclist.h"

#include "rc.h"
#include "rclist.h"

#pragma mark - tests

static void _empty_list(unit_state *state) {
    rclist *lst = rclist_cons(NULL, NULL);
    assert(state, lst, "must not be NULL");
    assert_eq(state, NULL, lst->head);
    assert_eq(state, NULL, lst->tail);
    rc_release(lst);
}

#pragma mark - suite

unit_test *create_rclist_tests() {
    return test_suite_alloc("rclist tests",
        test_alloc("empty list", _empty_list),
        NULL
    );
}
