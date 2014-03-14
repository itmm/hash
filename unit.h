#ifndef unit_h
#define unit_h

#include <stdbool.h>

#pragma mark - test management

    struct unit_test;

    typedef void (executor)(struct unit_test *test);

    typedef struct unit_test {
        const char *name;
        executor *setup;
        executor *run;
        executor *teardown;
        unsigned passed;
        unsigned failed;
    } unit_test;


    unit_test *test_alloc(const char *name, executor run);
    unit_test *test_full_alloc(const char *name, executor setup, executor run, executor teardown);
    unit_test *test_suite_alloc(const char *name, ...); // consumes tests
    void test_free(unit_test *test);

    void test_run(unit_test *test);

    void test_summary(unit_test *test);


#pragma mark - assertions

    #define assert(test, cond, ...) test_assert(test, cond, __FILE__, __LINE__, __VA_ARGS__)
    #define assert_eq_str(test, str1, str2) test_assert(test, strcmp(str1, str2) == 0, __FILE__, __LINE__, #str1 " (\"%s\") != " #str2 " (\"%s\")", str1, str2)
    
    void test_assert(unit_test *test, bool condition, const char *file, int line, const char *format, ...) __attribute__((format(printf, 5, 6)));

#endif
