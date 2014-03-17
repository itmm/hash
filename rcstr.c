#include "rcstr.h"

#include <string.h>

#include "log.h"
#include "rc.h"

rcstr rcstr_dup(const char *src) {
    void *result = NULL;
    if (src) {
        size_t count = strlen(src) + 1;
        result = rc_alloc(count, NULL);
        if (result) {
            memcpy(result, src, count);
        } else {
            log_error("can't alloc str");
        }
    } else {
        log_error("rcstr is NULL");
    }
    return result;
}
