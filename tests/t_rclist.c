#include "t_rclist.h"

#include "rc.h"
#include "rclist.h"
#include "rcstr.h"

#pragma mark - tests

static void _empty_list(unit_state *state) {
    rclist *lst = rclist_cons(NULL, NULL, NULL);
    assert(state, lst, "must not be NULL");
    assert_eq(state, NULL, lst->key);
    assert_eq(state, NULL, lst->value);
    assert_eq(state, NULL, lst->next);
    rc_release(lst);
}

static void _empty_list_string(unit_state *state) {
    rclist *lst = rclist_cons(NULL, NULL, NULL);
    rcstr s = rclist2str(lst);
    rc_release(lst);
    assert_eq_str(state, ":", s);
    rc_release(s);
}

static void _simple_list(unit_state *state) {
    rcstr key = rcstr_dup("key");
    rcstr value = rcstr_dup("value");
    rclist *lst = rclist_cons(key, value, NULL);
    rc_release(key);
    rc_release(value);
    assert(state, lst, "must not be NULL");
    assert_eq_str(state, "key", lst->key);
    assert_eq_str(state, "value", lst->value);
    assert_eq(state, NULL, lst->next);
    rc_release(lst);
}

static void _simple_list_string(unit_state *state) {
    rcstr key = rcstr_dup("key");
    rcstr value = rcstr_dup("value");
    rclist *lst = rclist_cons(key, value, NULL);
    rc_release(key);
    rc_release(value);
    rcstr s = rclist2str(lst);
    rc_release(lst);
    assert_eq_str(state, "key:value", s);
    rc_release(s);
}

static void _strange_list_string(unit_state *state) {
    rcstr key = rcstr_dup("key");
    rcstr value = rcstr_dup("value");
    rcstr rest = rcstr_dup("rest");
    rclist *lst = rclist_cons(key, value, rest);
    rc_release(key);
    rc_release(value);
    rc_release(rest);
    rcstr s = rclist2str(lst);
    rc_release(lst);
    assert_eq_str(state, "key:value . rest", s);
    rc_release(s);
}

static void _double_list_string(unit_state *state) {
    rcstr key_1 = rcstr_dup("a");
    rcstr value_1 = rcstr_dup("x");
    rclist *tail = rclist_cons(key_1, value_1, NULL);
    rc_release(key_1);
    rc_release(value_1);
    
    rcstr key_2 = rcstr_dup("b");
    rcstr value_2 = rcstr_dup("y");
    rclist *head = rclist_cons(key_2, value_2, tail);
    rc_release(tail);
    
    rcstr s = rclist2str(head);
    rc_release(head);
    assert_eq_str(state, "b:y a:x", s);
    rc_release(s);
}

static void _colon_key(unit_state *state) {
    rcstr key = rcstr_dup(":[");
    rcstr value = rcstr_dup("value");
    rclist *lst = rclist_cons(key, value, NULL);
    rc_release(key);
    rc_release(value);
    rcstr s = rclist2str(lst);
    rc_release(lst);
    assert_eq_str(state, "[:\\[]:value", s);
    rc_release(s);
}

#pragma mark - suite

unit_test *create_rclist_tests() {
    return test_suite_alloc(7, (unit_test *[]) {
        test_alloc(_empty_list),
        test_alloc(_empty_list_string),
        test_alloc(_simple_list),
        test_alloc(_simple_list_string),
        test_alloc(_strange_list_string),
        test_alloc(_double_list_string),
        test_alloc(_colon_key)
    });
}
