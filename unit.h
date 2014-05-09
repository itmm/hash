#ifndef unit_h
#define unit_h

#include <setjmp.h>
#include <stdbool.h>
#include <string.h>

#pragma mark - state management

    typedef struct unit_state unit_state;

    unit_state *state_alloc();
    void state_free(unit_state *state);

    void *state_context(unit_state *state);
    void state_set_context(unit_state *state, void *context);
    unsigned state_count(unit_state *state);
    unsigned state_failed(unit_state *state);
    bool state_succeeded(unit_state *state);

    void test_summary(unit_state *state);


#pragma mark - unit test

    typedef struct unit_test unit_test;

    typedef void (executor)(unit_state *test);
    typedef void (context_dealloc)(void *context);

    unit_test *test_alloc(executor run);
    unit_test *test_alloc_with_context(executor run, void *context, context_dealloc deallocator);
    unit_test *test_alloc_with_wrappers(executor setup, executor run, executor teardown);
    unit_test *test_full_alloc(executor setup, executor run, executor teardown, void *context, context_dealloc deallocator);
    void test_free(unit_test *test);

    void test_run(unit_test *test, unit_state *state);


#pragma mark - suites

    unit_test *test_suite_alloc(size_t count, unit_test *tests[count]); // consumes tests


#pragma mark - assertions

    #define assert(state, cond, ...) test_assert(state, cond, __FILE__, __LINE__, __func__, __VA_ARGS__)
    #define assert_eq(state, a, b) test_assert( \
        state, (a) == (b), __FILE__, __LINE__, __func__, #a " (%d) != " #b " (%d)", (int) (a), (int) (b))
    #define assert_eq_str(state, str1, str2) test_assert( \
        state, strcmp(str1, str2) == 0, __FILE__, __LINE__, __func__, #str1 " (\"%s\") != " #str2 " (\"%s\")", str1, str2)
    
    void test_assert(
        unit_state *state,
        bool condition,
        const char *file,
        int line,
        const char *function,
        const char *format, ...
    ) __attribute__((format(printf, 6, 7)));


#endif
