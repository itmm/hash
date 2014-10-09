#include "t_unit.h"

#include "log.h"

#include <stdlib.h>

#pragma mark - disable log messages

static log_handler *old_handler;

static void disable_logging(unit_state *state) {
    old_handler = log_set_handler(NULL);
}

static void reset_logging(unit_state *state) {
    log_set_handler(old_handler);
}


#pragma mark - test runner

typedef struct {
    unit_test *test;
    int expected_failures;
    int expected_count;
} runner_context;

static void run_test(unit_state *state) {
    return_unless(, state);
    
    runner_context *context = state_context(state);
    return_unless(, context);

    unit_state *wrapped_state = state_alloc();
    disable_logging(wrapped_state);
    test_run(context->test, wrapped_state);
    reset_logging(wrapped_state);
    
    assert_eq(state, context->expected_failures, state_failed(wrapped_state));
    assert_eq(state, context->expected_count, state_count(wrapped_state));
    
    state_free(wrapped_state);
}

static void dealloc_wrapper_context(void *context) {
    return_unless(, context);

    test_free(((runner_context *) context)->test);
    free(context);
}

static unit_test *create_test_runner(unit_test *test, int expected_failures, int expected_count) { // consumes test
    runner_context *context = malloc(sizeof(runner_context));
    if (context) {
        unit_test *wrapper = test_alloc_with_context(run_test, context, dealloc_wrapper_context);
        if (wrapper) {
            context->test = test;
            context->expected_failures = expected_failures;
            context->expected_count = expected_count;
            return wrapper;
        } else {
            dealloc_wrapper_context(context);
            log_error("can't allocate runner context");
        }
    } else {
        log_error("can't allocate runner");
    }
    test_free(test);
    return NULL;
}

#pragma mark - tests

static void _double_assert(unit_state *state) {
    assert_eq(state, 0, 1);
    assert_eq(state, 1, 2);
}

#pragma mark - suite

unit_test *create_unit_tests() {
    return test_suite_alloc(1, (unit_test *[]) {
        create_test_runner(test_alloc(_double_assert), 1, 1)
    });
}
