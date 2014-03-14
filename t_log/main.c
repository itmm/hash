#include "log.h"
#include "unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma mark - log mock

static log_handler *_oldLogHandler;
static char *_gotMessage;
static const char *_gotType;

const size_t max_log_size = 1024;

static void _testing_log_handler(const char *file, int line, const char *type, const char *format, va_list parameters) {
    if (!_gotMessage) {
        _gotMessage = malloc(max_log_size);
        if (_gotMessage) {
            int size = vsnprintf(_gotMessage, max_log_size, format, parameters);
            if (size >= max_log_size) {
                log_error_with_handler(log_default_handler, "log message too big %d >= %lu", size, max_log_size);
            }
        } else {
            log_error_with_handler(log_default_handler, "can't allocate log message");
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

static void _setup_single_logger(unit_test *test) {
    _oldLogHandler = log_set_handler(_testing_log_handler);
    _gotMessage = NULL;
    _gotType = NULL;
}

static void _teardown_single_logger(unit_test *test) {
    log_set_handler(_oldLogHandler);
    if (_gotMessage) {
        free(_gotMessage);
        _gotMessage = NULL;
    }
}


#pragma mark - tests

static void _log_empty_message(unit_test *test) {
    log_info("");
    assert_eq_str(test, "", _gotMessage);
}

static void _log_formatted_message(unit_test *test) {
    log_error("n: %d, s: %s", 3, "abc");
    assert_eq_str(test, "n: 3, s: abc", _gotMessage);
}

static void _log_info_message(unit_test *test) {
    log_info("");
    assert_eq_str(test, "info", _gotType);
}

static void _log_error_message(unit_test *test) {
    log_error("");
    assert_eq_str(test, "ERROR", _gotType);
}


#pragma mark - suite

int main(int argc, const char * argv[])
{
    unit_test *suite = test_suite_alloc("log tests",
        test_full_alloc("log empty message", _setup_single_logger, _log_empty_message, _teardown_single_logger),
        test_full_alloc("log formatted message", _setup_single_logger, _log_formatted_message, _teardown_single_logger),
        test_full_alloc("log info message", _setup_single_logger, _log_info_message, _teardown_single_logger),
        test_full_alloc("log error message", _setup_single_logger, _log_error_message, _teardown_single_logger),
        NULL
    );
    
    if (!suite) {
        log_error("can't alloc suite");
        exit(10);
    }
    test_run(suite);
    test_summary(suite);
    test_free(suite);
    return 0;
}
