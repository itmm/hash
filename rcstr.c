#include "rcstr.h"

#include <string.h>

#include "log.h"
#include "rc.h"

rcstr rcstr_dup(const char *src) {
    return_null_value(src, NULL);
    
    size_t count = strlen(src) + 1;
    void *result = rc_alloc(count, NULL);
    return_null_value(result, NULL);
    
    memcpy(result, src, count);
    
    return result;
}
