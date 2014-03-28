#include "t_rcstr.h"

#include "rc.h"
#include "rcstr.h"


#pragma mark - tests

static inline void _test_string(unit_state *state, const char *str) {
    rcstr rc = rcstr_dup(str);
    assert_eq_str(state, str, rcstr_str(rc));
    rc_release(rc);
}

static inline void _test_quote(unit_state *state, const char *str, const char *expected) {
    rcstr rc = rcstr_dup(str);
    rcstr rep = rc2str(rc);
    rc_release(rc);
    assert_eq_str(state, expected, rcstr_str(rep));
    rc_release(rep);
}

static void _empty_string(unit_state *state) {
    _test_string(state, "");
}

static void _some_string(unit_state *state) {
    _test_string(state, "abc");
}

static void _simple_quote(unit_state *state) {
    _test_quote(state, "abc", "abc");
}

static void _colon_quote(unit_state *state) {
    _test_quote(state, "a:b", "[a:b]");
}

static void _space_quote(unit_state *state) {
    _test_quote(state, "a b", "[a b]");
}

static void _parenthesis_quote(unit_state *state) {
    _test_quote(state, "a()b", "[a()b]");
}

static void _backslash_quote(unit_state *state) {
    _test_quote(state, "a\\b", "[a\\\\b]");
}

static void _starting_backslash_quote(unit_state *state) {
    _test_quote(state, "\\b", "[\\\\b]");
}

static void _ending_backslash_quote(unit_state *state) {
    _test_quote(state, "a\\", "[a\\\\]");
}

static void _double_backslash_quote(unit_state *state) {
    _test_quote(state, "a\\\\b", "[a\\\\\\\\b]");
}

static void _multiple_backslashes_quote(unit_state *state) {
    _test_quote(state, "a\\b\\c", "[a\\\\b\\\\c]");
}

static void _matching_quotes(unit_state *state) {
    _test_quote(state, "[a[b[]]c]d", "[[a[b[]]c]d]");
}

static void _unmatched_opening_quote(unit_state *state) {
    _test_quote(state, "[a[b[]]cd", "[\\[a[b[]]cd]");
}

static void _unmatched_closing_quote(unit_state *state) {
    _test_quote(state, "a[b[]]c]d", "[a[b[]]c\\]d]");
}

static void _inverse_quotes(unit_state *state) {
    _test_quote(state, "][", "[\\]\\[]");
}

static void _opening_quote(unit_state *state) {
    _test_quote(state, "[", "[\\[]");
}

static void _closing_quote(unit_state *state) {
    _test_quote(state, "]", "[\\]]");
}

#pragma mark - suite

unit_test *create_rcstr_tests() {
    return test_suite_alloc(17, (unit_test *[]) {
        test_alloc(_empty_string),
        test_alloc(_some_string),
        test_alloc(_simple_quote),
        test_alloc(_colon_quote),
        test_alloc(_space_quote),
        test_alloc(_parenthesis_quote),
        test_alloc(_backslash_quote),
        test_alloc(_starting_backslash_quote),
        test_alloc(_ending_backslash_quote),
        test_alloc(_double_backslash_quote),
        test_alloc(_multiple_backslashes_quote),
        test_alloc(_matching_quotes),
        test_alloc(_unmatched_opening_quote),
        test_alloc(_unmatched_closing_quote),
        test_alloc(_inverse_quotes),
        test_alloc(_opening_quote),
        test_alloc(_closing_quote)
    });
}
