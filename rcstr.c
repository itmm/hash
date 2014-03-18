#include "rcstr.h"

#include <string.h>

#include "log.h"
#include "rc.h"
#include "rclist.h"

rcstr rcstr_dup(const char *src) {
    return rcstr_dups(1, (const char *[]) { src });
}

rcstr rcstr_dups(size_t count, const char *srcs[count]) {
    size_t length = 1;
    const char **begin = srcs;
    const char **end = begin + count;
    for (const char **cur = begin; cur != end; ++cur) {
        if (*cur) { length += strlen(*cur); }
    }

    void *result = rc_alloc(length, rc_type_string, NULL);
    return_value_unless(result, NULL);

    char *dest = result;
    for (const char **cur = begin; cur != end; ++cur) {
        if (*cur) {
            size_t len = strlen(*cur);
            memcpy(dest, *cur, len);
            dest += len;
        }
    }
    *dest = 0;
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
