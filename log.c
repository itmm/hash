#include "log.h"

#include <stdio.h>

void log_default_handler(
    const char *file,
    int line,
    const char *function,
    const char *type,
    const char *format,
    va_list parameters
) {
    fprintf(stderr, "%s: %s[%s:%d] ", type, function, file, line);
    vfprintf(stderr, format, parameters);
    fputc('\n', stderr);
}

static log_handler *_current_handler = log_default_handler;

void log_message_with_handler_list(
    log_handler *handler,
    const char *file,
    int line,
    const char *function,
    const char *type,
    const char *format,
    va_list parameters
) {
    if (handler) {
        handler(file, line, function, type, format, parameters);
    }
}

void log_message_with_handler(
    log_handler *handler,
    const char *file,
    int line,
    const char *function,
    const char *type,
    const char *format,
    ...
) {
    va_list parameters;
    va_start(parameters, format);
    log_message_with_handler_list(handler, file, line, function, type, format, parameters);
    va_end(parameters);
}

void log_message(const char *file, int line, const char *function, const char *type, const char *format, ...) {
    va_list parameters;
    va_start(parameters, format);
    log_message_with_handler_list(_current_handler, file, line, function, type, format, parameters);
    va_end(parameters);
}

log_handler *log_set_handler(log_handler *handler) {
    log_handler *old_handler = _current_handler;
    _current_handler = handler;
    return old_handler;
}
