#include "unit.h"

#include "log.h"

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unit_test **begin;
    unit_test **end;
} unit_suite_context;

static void _test_suite_run(unit_state *state) {
    return_unless(state);

    --state->count; // suite is no test
    
    unit_suite_context *suite = state->context;
    return_unless(suite);
    for (unit_test **cur = suite->begin; cur != suite->end; ++cur) {
        if (*cur) {
            test_run(*cur, state);
        } else {
            log_error_with_handler(log_default_handler, "test in suite is NULL");
        }
    }
}

static void _test_suite_dealloc(void *context) {
    return_unless(context);
    
    unit_suite_context *suite = context;
    if (suite->begin) {
        for (unit_test **cur = suite->begin; cur != suite->end; ++cur) {
            test_free(*cur);
        }
        free(suite->begin);
    }
    free(context);
}

unit_test *test_alloc(const char *name, executor run) {
    return test_full_alloc(name, NULL, run, NULL);
}

unit_test *test_full_alloc(const char *name, executor setup, executor run, executor teardown) {
    if (!name) log_info("name of unit_test is NULL");
    if (!run) log_info("test of unit_test is NULL");
    
    unit_test *result = malloc(sizeof(unit_test));
    return_value_unless(result, NULL);

    result->name = name;
    result->setup = setup;
    result->run = run;
    result->teardown = teardown;
    result->dealloc = NULL;

    return result;
}

size_t number_of_tests(va_list tests) {
    va_list tmp;
    va_copy(tmp, tests);
        
    size_t count = 0;
    for (; va_arg(tmp, unit_test *); ++count) {}
    return count;
}

unit_test *test_suite_alloc(const char *name, ...) {
    if (!name) log_info("name of unit_test is NULL");
    unit_test *test = test_full_alloc(name, NULL, _test_suite_run, NULL);
    if (!test) { log_error("can't alloc suite test"); return NULL; }
    unit_suite_context *suite = malloc(sizeof(unit_suite_context));
    if (suite) {
        va_list tests;
        va_start(tests, name);
        size_t count = number_of_tests(tests);

        test->context = suite;
        test->dealloc = _test_suite_dealloc;
        if (count) {
            suite->begin = malloc(count * sizeof(unit_test *));
            if (suite->begin) {
                suite->end = suite->begin;
                for (;;) {
                    unit_test *test = va_arg(tests, unit_test *);
                    if (!test) break;
                    *suite->end++ = test;
                }
            } else {
                log_error("can't alloc tests for unit_suite %s", name);
                suite->begin = suite->end = NULL;
                test_free(test);
                test = NULL;
            }
        } else {
            suite->begin = suite->end = NULL;
        }
        
        va_end(tests);
    } else {
        test_free(test);
        test = NULL;
        log_error("can't alloc unit_suite %s", name);
    }
    return test;
}

void test_free(unit_test *test) {
    return_unless(test);

    if (test->dealloc) {
        test->dealloc(test->context);
    }
    free(test);
}

void test_run(unit_test *test, unit_state *state) {
    return_unless(state);
    return_unless(test);

    state->context = test->context;
    ++state->count;
    
    if (test->setup) test->setup(state);
    if (!setjmp(state->env)) {
        if (test->run) test->run(state);
    }
    if (test->teardown) test->teardown(state);
}

void test_summary(unit_state *state) {
    return_unless(state);
    if (state->failed) {
        printf("%u UNIT-TESTS FAILED (from %u tests)\n", state->failed, state->count);
    } else {
        printf("all %u tests passed.\n", state->count);
    }
}


void test_assert(unit_state *state, bool condition, const char *file, int line, const char *function, const char *format, ...) {
    if (!condition) {
        return_unless(state);
        ++state->failed;
        
        va_list parameters;
        va_start(parameters, format);
        log_message_with_list(file, line, function, "UNIT-TEST FAILED", format, parameters);
        va_end(parameters);
        
        longjmp(state->env, 1);
    }
}

