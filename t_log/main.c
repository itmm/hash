#include "log.h"
#include "unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static log_handler *_oldLogHandler;
static char *_gotMessage;

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
        log_error_with_handler(log_default_handler, "already logged %s", _gotMessage);
    }
}

static void _setup_single_logger(unit_test *test) {
    _oldLogHandler = log_set_handler(_testing_log_handler);
    _gotMessage = NULL;
}

static void _teardown_single_logger(unit_test *test) {
    log_set_handler(_oldLogHandler);
    if (_gotMessage) {
        free(_gotMessage);
        _gotMessage = NULL;
    }
}

static void _log_empty_message(unit_test *test) {
    log_info("");
    assert_eq_str(test, "", _gotMessage);
}

int main(int argc, const char * argv[])
{
    unit_test *suite = test_suite_alloc("log tests",
        test_full_alloc("log empty message", _setup_single_logger, _log_empty_message, _teardown_single_logger)
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
