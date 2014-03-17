#ifndef log_h
#define log_h

#include <stdarg.h>

#pragma mark - logging

    #define log_info(...) log_message(__FILE__, __LINE__, __func__, "info", __VA_ARGS__)
    #define log_error(...) log_message(__FILE__, __LINE__, __func__, "ERROR", __VA_ARGS__)
    #define log_info_with_handler(handler, ...) \
        log_message_with_handler(handler, __FILE__, __LINE__, __func__, "info", __VA_ARGS__)
    #define log_error_with_handler(handler, ...) \
        log_message_with_handler(handler, __FILE__, __LINE__, __func__, "ERROR", __VA_ARGS__)

    #define return_null(param) if (!param) { \
        log_message(__FILE__, __LINE__, __func__, "ERROR", #param " must not be NULL"); \
        return; \
    }
    #define return_null_value(param, ret_val) if (!param) { \
        log_message(__FILE__, __LINE__, __func__, "ERROR", #param " must not be NULL"); \
        return ret_val; \
    }

    void log_message(
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format,
        ...
    ) __attribute__((format(printf, 5, 6)));

    void log_message_with_list(
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format,
        va_list parameters
    );

#pragma mark - switch log handler

    typedef void (log_handler)(
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format,
        va_list parameters
    );

    void log_default_handler(
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format,
        va_list parameters
    );

    log_handler *log_set_handler(log_handler *handler);

    void log_message_with_handler(
        log_handler *handler,
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format, ...
    ) __attribute__((format(printf, 6, 7)));

    void log_message_with_handler_list(
        log_handler *handler,
        const char *file,
        int line,
        const char *function,
        const char *type,
        const char *format,
        va_list parameters
    );

#endif
