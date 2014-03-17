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
    return_null(state);
    
    runner_context *context = state->context;
    return_null(context);

    unit_state wrapped_state = {0};
    disable_logging(&wrapped_state);
    test_run(context->test, &wrapped_state);
    reset_logging(&wrapped_state);
    
    assert_eq(state, context->expected_failures, wrapped_state.failed);
    assert_eq(state, context->expected_count, wrapped_state.count);
}

static void dealloc_wrapper_context(void *context) {
    return_null(context);

    test_free(((runner_context *) context)->test);
    free(context);
}

static unit_test *create_test_runner(unit_test *test, int expected_failures, int expected_count) { // consumes test
    unit_test *wrapper = test_alloc(test->name, run_test);
    if (wrapper) {
        runner_context *context = malloc(sizeof(runner_context));
        if (context) {
            context->test = test;
            context->expected_failures = expected_failures;
            context->expected_count = expected_count;
            wrapper->context = context;
            wrapper->dealloc = dealloc_wrapper_context;
            return wrapper;
        } else {
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
    return test_suite_alloc("unit tests",
        create_test_runner(test_alloc("double assert", _double_assert), 1, 1),
        NULL
    );
}
