#include "t_bool.h"

#include "log.h"
#include "rc.h"

static void _true(unit_state *state) {
    void *t = rc_true();
    return_unless(t);
    assert_eq(state, rc_get_type(t), rc_type_true);
}

static void _false(unit_state *state) {
    void *f = rc_false();
    return_unless(f);
    assert_eq(state, rc_get_type(f), rc_type_false);
}

static void _hashes(unit_state *state) {
    void *t = rc_true();
    return_unless(t);
    
    void *f = rc_false();
    return_unless(f);
    
    int ht = rc_hash(t);
    int hf = rc_hash(f);
    assert(state, ht != hf, "hashes must differ %d == %d", ht, hf);
}

#pragma mark - suite

unit_test *create_bool_tests() {
    return test_suite_alloc(3, (unit_test *[]) {
        test_alloc(_true),
        test_alloc(_false),
        test_alloc(_hashes)
    });
}
