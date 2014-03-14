#ifndef log_h
#define log_h

#include <stdarg.h>

#pragma mark - logging

    #define log_info(...) log_message(__FILE__, __LINE__, "info", __VA_ARGS__)
    #define log_error(...) log_message(__FILE__, __LINE__, "ERROR", __VA_ARGS__)
    #define log_info_with_handler(handler, ...) log_message_with_handler(handler, __FILE__, __LINE__, "info", __VA_ARGS__)
    #define log_error_with_handler(handler, ...) log_message_with_handler(handler, __FILE__, __LINE__, "ERROR", __VA_ARGS__)

    void log_message(const char *file, int line, const char *type, const char *format, ...) __attribute__((format(printf, 4, 5)));

#pragma mark - switch log handler

    typedef void (log_handler)(const char *file, int line, const char *type, const char *format, va_list parameters);

    void log_default_handler(const char *file, int line, const char *type, const char *format, va_list parameters);

    log_handler *log_set_handler(log_handler *handler);

    void log_message_with_handler(
        log_handler *handler,
        const char *file,
        int line,
        const char *type,
        const char *format, ...
    ) __attribute__((format(printf, 5, 6)));

    void log_message_with_handler_list(
        log_handler *handler,
        const char *file,
        int line,
        const char *type,
        const char *format,
        va_list parameters
    );

#endif
