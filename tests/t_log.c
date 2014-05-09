#include "t_log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"


#pragma mark - log mock

static log_handler *_oldLogHandler;
static char *_gotMessage;
static const char *_gotType;

static void _testing_log_handler(
    const char *file,
    int line,
    const char *function,
    const char *type,
    const char *format,
    va_list parameters
) {
    if (!_gotMessage) {
        va_list tmp;
        va_copy(tmp, parameters);
        size_t needed_size = vsnprintf(NULL, 0, format, tmp) + 1;
        _gotMessage = malloc(needed_size);
        if (_gotMessage) {
            vsnprintf(_gotMessage, needed_size, format, parameters);
        } else {
            log_error_with_handler(log_default_handler, "can't allocate log message (%lu bytes)", needed_size);
        }
    } else {
        log_error_with_handler(log_default_handler, "already logged message %s", _gotMessage);
    }
    
    if (!_gotType) {
        _gotType = type;
    } else {
        log_error_with_handler(log_default_handler, "already logged type %s", _gotType);
    }
}

static void _setup_single_logger(unit_state *state) {
    _oldLogHandler = log_set_handler(_testing_log_handler);
    _gotMessage = NULL;
    _gotType = NULL;
}

static void _teardown_single_logger(unit_state *state) {
    log_set_handler(_oldLogHandler);
    if (_gotMessage) {
        free(_gotMessage);
        _gotMessage = NULL;
    }
}


#pragma mark - tests

static void _log_empty_message(unit_state *state) {
    log_info("");
    assert_eq_str(state, "", _gotMessage);
}

static void _log_formatted_message(unit_state *state) {
    log_error("n: %d, s: %s", 3, "abc");
    assert_eq_str(state, "n: 3, s: abc", _gotMessage);
}

static void _log_info_message(unit_state *state) {
    log_info("");
    assert_eq_str(state, "info", _gotType);
}

static void _log_error_message(unit_state *state) {
    log_error("");
    assert_eq_str(state, "ERROR", _gotType);
}


#pragma mark - suite

unit_test *create_log_tests() {
    return test_suite_alloc(4, (unit_test *[]) {
        test_alloc_with_wrappers(_setup_single_logger, _log_empty_message, _teardown_single_logger),
        test_alloc_with_wrappers(_setup_single_logger, _log_formatted_message, _teardown_single_logger),
        test_alloc_with_wrappers(_setup_single_logger, _log_info_message, _teardown_single_logger),
        test_alloc_with_wrappers(_setup_single_logger, _log_error_message, _teardown_single_logger)
    });
}
