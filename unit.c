#include "unit.h"

#include "log.h"

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

    struct unit_state {
        void *context;
        unsigned count;
        unsigned failed;
        jmp_buf env;
    };

    typedef void (executor)(unit_state *test);

    struct unit_test {
        void *context;
        executor *setup;
        executor *run;
        executor *teardown;
        void (*dealloc)(void *context);
    };

typedef struct {
    unit_test **begin;
    unit_test **end;
} unit_suite_context;

unit_state *state_alloc() {
    unit_state *state = malloc(sizeof(unit_state));
    bzero(state, sizeof(unit_state));
    return_value_unless(state, NULL);
    return state;
}

void state_free(unit_state *state) {
    free(state);
}

bool state_succeeded(unit_state *state) {
    return state_failed(state) == 0 && state_count(state);
}

unsigned state_count(unit_state *state) {
    return_value_unless(state, 0);
    return state->count;
}

unsigned state_failed(unit_state *state) {
    return_value_unless(state, 0);
    return state->failed;
}

void *state_context(unit_state *state) {
    return_value_unless(state, NULL);
    return state->context;
}

void state_set_context(unit_state *state, void *context) {
    return_unless(state);
    state->context = context;
}

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

unit_test *test_alloc(executor run) {
    return test_full_alloc(NULL, run, NULL, NULL, NULL);
}

unit_test *test_alloc_with_context(executor run, void *context, context_dealloc deallocator) {
    return test_full_alloc(NULL, run, NULL, context, deallocator);
}

unit_test *test_alloc_with_wrappers(executor setup, executor run, executor teardown) {
    return test_full_alloc(setup, run, teardown, NULL, NULL);
}

unit_test *test_full_alloc(executor setup, executor run, executor teardown, void *context, context_dealloc deallocator) {
    if (!run) log_info("test of unit_test is NULL");
    
    unit_test *result = malloc(sizeof(unit_test));
    return_value_unless(result, NULL);

    result->setup = setup;
    result->run = run;
    result->teardown = teardown;
    result->context = context;
    result->dealloc = deallocator;

    return result;
}

size_t number_of_tests(va_list tests) {
    va_list tmp;
    va_copy(tmp, tests);
        
    size_t count = 0;
    for (; va_arg(tmp, unit_test *); ++count) {}
    return count;
}

unit_test *test_suite_alloc(size_t count, unit_test *tests[count]) {

    unit_suite_context *suite = malloc(sizeof(unit_suite_context));
    return_value_unless(suite, NULL);
    
    unit_test *test = test_alloc_with_context(_test_suite_run, suite, _test_suite_dealloc);
    if (test) {
        if (count) {
            size_t size = count * sizeof(unit_test *);
            suite->begin = malloc(size);
            if (suite->begin) {
                suite->end = suite->begin + count;
                memcpy(suite->begin, tests, size);
            } else {
                log_error("can't alloc tests for unit_suite");
                suite->begin = suite->end = NULL;
                test_free(test);
                test = NULL;
            }
        } else {
            suite->begin = suite->end = NULL;
        }
    } else {
        _test_suite_dealloc(suite);
        test = NULL;
        log_error("can't alloc unit_suite");
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

