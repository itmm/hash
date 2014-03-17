#ifndef unit_h
#define unit_h

#include <stdbool.h>
#include <string.h>

#pragma mark - test management

    struct unit_test;

    typedef struct {
        struct unit_test *current_test;
        unsigned count;
        unsigned failed;
    } unit_state;

    typedef void (executor)(unit_state *test);

    typedef struct unit_test {
        const char *name;
        void *context;
        executor *setup;
        executor *run;
        executor *teardown;
        void (*dealloc)(void *context);
    } unit_test;


    unit_test *test_alloc(const char *name, executor run);
    unit_test *test_full_alloc(const char *name, executor setup, executor run, executor teardown);
    unit_test *test_suite_alloc(const char *name, ...); // consumes tests
    void test_free(unit_test *test);

    void test_run(unit_test *test, unit_state *state);

    void test_summary(unit_state *state);


#pragma mark - assertions

    #define assert(state, cond, ...) test_assert(state, cond, __FILE__, __LINE__, __VA_ARGS__)
    #define assert_eq(state, a, b) test_assert(state, a == b, __FILE__, __LINE__, #a " != " #b)
    #define assert_eq_str(state, str1, str2) test_assert(state, strcmp(str1, str2) == 0, __FILE__, __LINE__, #str1 " (\"%s\") != " #str2 " (\"%s\")", str1, str2)
    
    void test_assert(unit_state *state, bool condition, const char *file, int line, const char *format, ...) __attribute__((format(printf, 5, 6)));

#endif
