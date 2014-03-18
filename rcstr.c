#include "rcstr.h"

#include <stdarg.h>
#include <string.h>

#include "log.h"
#include "rc.h"
#include "rclist.h"

rcstr rcstr_dup(const char *src) {
    return_value_unless(src, NULL);
    
    size_t count = strlen(src) + 1;
    void *result = rc_alloc(count, rc_type_string, NULL);
    return_value_unless(result, NULL);
    
    memcpy(result, src, count);
    
    return result;
}

rcstr rcstr_dups(const char *first, ...) {
    size_t count = strlen(first) + 1;
    va_list strs;
    va_start(strs, first);
    va_list tmp;
    va_copy(tmp, strs);
    for (const char *cur; (cur = va_arg(tmp, const char *));) {
        count += strlen(cur);
    }
    va_end(tmp);

    void *result = rc_alloc(count, rc_type_string, NULL);
    return_value_unless(result, NULL);

    size_t len = strlen(first);
    memcpy(result, first, len);
    char *dest = result + len;
    for (const char *cur; (cur = va_arg(strs, const char *));) {
        len = strlen(cur);
        memcpy(dest, cur, len);
        dest += len;
    }
    *dest = 0;
    va_end(strs);
    
    return result;
}

rcstr rc2str(void *rc) {
    if (!rc) {
        return rcstr_dup("");
    }
    switch (rc_get_type(rc)) {
        case rc_type_string: return rc_retain(rc);
        case rc_type_list: return rclist2str((rclist *) rc);
        default:
            log_error("can't stringify type %d", (int) rc_get_type(rc));
            return rcstr_dup("???");
    }
}
