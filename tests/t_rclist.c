#include "t_rclist.h"

#include "log.h"
#include "rc.h"
#include "rclist.h"
#include "rcstr.h"

#pragma mark - tests

static rclist create_node(const char *key, const char *value, void *next) {
    rcstr rckey = key ? rcstr_dup(key) : NULL;
    rcstr rcvalue = value ? rcstr_dup(value) : NULL;
    rclist result = rclist_cons(rckey, rcvalue, next);
    rc_release(rckey);
    rc_release(rcvalue);
    return_value_unless(result, NULL);
    return result;
}

static void _empty_list(unit_state *state) {
    rclist lst = create_node(NULL, NULL, NULL);
    return_unless(lst);

    assert_eq(state, NULL, rclist_key(lst));
    assert_eq(state, NULL, rclist_value(lst));
    assert_eq(state, NULL, rclist_next(lst));

    rc_release(lst);
}

static void _test_str(unit_state *state, rclist *lst, const char *expected) {
    return_unless(lst);
    
    rcstr got = rc2str(lst);
    return_unless(got);
    
    rc_release(lst);
    assert_eq_str(state, expected, rcstr_str(got));
    rc_release(got);
}

static void _empty_list_string(unit_state *state) {
    _test_str(state, create_node(NULL, NULL, NULL), ":");
}

static void _simple_list(unit_state *state) {
    rclist lst = create_node("key", "value", NULL);
    return_unless(lst);
    
    assert_eq_str(state, "key", rcstr_str(rclist_key(lst)));
    assert_eq_str(state, "value", rcstr_str(rclist_value(lst)));
    assert_eq(state, NULL, rclist_next(lst));

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
    rclist tail = create_node("a", "x", NULL);
    _test_str(state, create_node("b", "y", tail), "b:y a:x");
    rc_release(tail);
}

static void _colon_key(unit_state *state) {
    _test_str(state, create_node(":[", "value", NULL), "[:\\[]:value");
}

static void _hash_of_empty_list(unit_state *state) {
    rclist lst = create_node(NULL, NULL, NULL);
    int h = rc_hash(lst);
    rc_release(lst);
    assert_eq(state, 0, h);
}

static void _different_hash_for_different_keys(unit_state *state) {
    rclist a = create_node("a", NULL, NULL);
    rclist b = create_node("b", NULL, NULL);
    int ha = rc_hash(a);
    rc_release(a);
    int hb = rc_hash(b);
    rc_release(b);
    assert(state, ha != hb, "hashes don't differ %d == %d", ha, hb);
}

#pragma mark - suite

unit_test *create_rclist_tests() {
    return test_suite_alloc(9, (unit_test *[]) {
        test_alloc(_empty_list),
        test_alloc(_empty_list_string),
        test_alloc(_simple_list),
        test_alloc(_simple_list_string),
        test_alloc(_strange_list_string),
        test_alloc(_double_list_string),
        test_alloc(_colon_key),
        test_alloc(_hash_of_empty_list),
        test_alloc(_different_hash_for_different_keys)
    });
}
