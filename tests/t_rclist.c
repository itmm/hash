#include "t_rclist.h"

#include "log.h"
#include "rc.h"
#include "rclist.h"
#include "rcstr.h"

#pragma mark - tests

static rclist *create_node(const char *key, const char *value, void *next) {
    rcstr rckey = key ? rcstr_dup(key) : NULL;
    rcstr rcvalue = value ? rcstr_dup(value) : NULL;
    rclist *result = rclist_cons(rckey, rcvalue, next);
    rc_release(rckey);
    rc_release(rcvalue);
    return_value_unless(result, NULL);
    return result;
}

static void _empty_list(unit_state *state) {
    rclist *lst = create_node(NULL, NULL, NULL);
    return_unless(lst);

    assert_eq(state, NULL, lst->key);
    assert_eq(state, NULL, lst->value);
    assert_eq(state, NULL, lst->next);

    rc_release(lst);
}

static void _test_str(unit_state *state, rclist *lst, const char *expected) {
    return_unless(lst);
    
    rcstr got = rclist2str(lst);
    return_unless(got);
    
    rc_release(lst);
    assert_eq_str(state, expected, got);
    rc_release(got);
}

static void _empty_list_string(unit_state *state) {
    _test_str(state, create_node(NULL, NULL, NULL), ":");
}

static void _simple_list(unit_state *state) {
    rclist *lst = create_node("key", "value", NULL);
    return_unless(lst);
    
    assert_eq_str(state, "key", lst->key);
    assert_eq_str(state, "value", lst->value);
    assert_eq(state, NULL, lst->next);

    rc_release(lst);
}

static void _simple_list_string(unit_state *state) {
    _test_str(state, create_node("key", "value", NULL), "key:value");
}

static void _strange_list_string(unit_state *state) {
    rcstr rest = rcstr_dup("rest");
    _test_str(state, create_node("key", "value", rest), "key:value . rest");
    rc_release(rest);
}

static void _double_list_string(unit_state *state) {
    rclist *tail = create_node("a", "x", NULL);
    _test_str(state, create_node("b", "y", tail), "b:y a:x");
    rc_release(tail);
}

static void _colon_key(unit_state *state) {
    _test_str(state, create_node(":[", "value", NULL), "[:\\[]:value");
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
