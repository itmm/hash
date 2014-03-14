#include "unit.h"

#include "log.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unit_test wrapper;
    unit_test **begin;
    unit_test **end;
} unit_suite;

static void _test_suite_run(unit_test *test) {
    unit_suite *suite = (unit_suite *) test;
    test->passed = 0;
    if (suite) {
        for (unit_test **cur = suite->begin; cur != suite->end; ++cur) {
            test_run(*cur);
            test->failed += (*cur)->failed;
            test->passed += (*cur)->passed;
        }
    }
}

static unit_suite *is_suite(unit_test *test) {
    return test->run == _test_suite_run ? (unit_suite *) test : NULL;
}

unit_test *test_alloc(const char *name, executor run) {
    return test_full_alloc(name, NULL, run, NULL);
}

unit_test *test_full_alloc(const char *name, executor setup, executor run, executor teardown) {
    if (!name) log_info("name of unit_test is NULL");
    if (!run) log_info("test of unit_test is NULL");
    unit_test *result = malloc(sizeof(unit_test));
    if (result) {
        result->name = name;
        result->setup = setup;
        result->run = run;
        result->teardown = teardown;
    } else {
        log_error("can't alloc unit_test %s", name);
    }
    return result;
}

unit_test *test_suite_alloc(const char *name, ...) {
    if (!name) log_info("name of unit_test is NULL");
    unit_suite *suite = malloc(sizeof(unit_suite));
    if (suite) {
        suite->wrapper.name = name;
        suite->wrapper.setup = NULL;
        suite->wrapper.run = _test_suite_run;
        suite->wrapper.teardown = NULL;
        
        size_t count = 0;
        va_list tests;
        va_start(tests, name);
        for (;;) {
            unit_test *test = va_arg(tests, unit_test *);
            if (!test) break;
            ++count;
        }
        va_end(tests);

        if (count) {
            suite->begin = malloc(count * sizeof(unit_test *));
            if (suite->begin) {
                suite->end = suite->begin;
                va_start(tests, name);
                for (;;) {
                    unit_test *test = va_arg(tests, unit_test *);
                    if (!test) break;
                    *suite->end++ = test;
                }
                va_end(tests);
            } else {
                log_error("can't alloc tests for unit_suite %s", name);
                suite->begin = suite->end = NULL;
            }
        } else {
            suite->begin = suite->end = NULL;
        }
    } else {
        log_error("can't alloc unit_suite %s", name);
    }
    return (unit_test *) suite;
}

void test_free(unit_test *test) {
    if (test) {
        unit_suite *suite = is_suite(test);
        if (suite) {
            for (unit_test **cur = suite->begin; cur != suite->end; ++cur) {
                free(*cur);
            }
            free(suite->begin);
        }
        free(test);
    } else {
        log_info("test_free is NULL");
    }
}

void test_run(unit_test *test) {
    if (test) {
        test->failed = 0;
        test->passed = 1;
        if (test->setup) test->setup(test);
        if (test->run) test->run(test);
        if (test->teardown) test->teardown(test);
    }
}

void test_summary(unit_test *test) {
    if (test) {
        if (test->failed) {
            printf("%u UNIT-TESTS FAILED (%u tests passed)\n", test->failed, test->passed);
        } else {
            printf("all %u tests passed.\n", test->passed);
        }
    } else {
        log_info("test is NULL");
    }
}


void test_assert(unit_test *test, bool condition, const char *file, int line, const char *format, ...) {
    if (!condition) {
        if (test) {
            ++test->failed;
            if (test->passed) {
                --test->passed;
            }
        } else {
            log_error("test is NULL");
        }
        va_list parameters;
        va_start(parameters, format);
        log_message_with_handler_list(log_default_handler, file, line, "UNIT-TEST FAILED", format, parameters);
        va_end(parameters);
    }
}

