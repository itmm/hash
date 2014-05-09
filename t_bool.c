#include "t_bool.h"

#include "log.h"
#include "rc.h"
#include "rcstr.h"

static void _true(unit_state *state) {
    void *t = rc_true();
    return_unless(t);
    assert_eq(state, rc_get_type(t), rc_type_true);
    rc_release(t);
}

static void _false(unit_state *state) {
    void *f = rc_false();
    return_unless(f);
    assert_eq(state, rc_get_type(f), rc_type_false);
    rc_release(f);
}

static void _hashes(unit_state *state) {
    void *t = rc_true();
    return_unless(t);
    
    void *f = rc_false();
    return_unless(f);
    
    int ht = rc_hash(t);
    int hf = rc_hash(f);
    assert(state, ht != hf, "hashes must differ %d == %d", ht, hf);
    
    rc_release(t);
    rc_release(f);
}

static void _names_match(unit_state *state, void *obj, const char *expected) {
    rcstr *name = rc2str(obj);
    assert_eq_str(state, rcstr_str(name), expected);
    rc_release(name);
    rc_release(obj);
}

static void _true_name(unit_state *state) {
    _names_match(state, rc_true(), "true");
}

static void _false_name(unit_state *state) {
    _names_match(state, rc_false(), "false");
}

#pragma mark - suite

unit_test *create_bool_tests() {
    return test_suite_alloc(5, (unit_test *[]) {
        test_alloc(_true),
        test_alloc(_false),
        test_alloc(_hashes),
        test_alloc(_true_name),
        test_alloc(_false_name)
    });
}
