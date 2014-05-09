#include "rcint.h"

#include "log.h"
#include "rc.h"

#include <limits.h>
#include <stdbool.h>

struct rcint {
    int value;
};

rcint *rcint_alloc(int value) {
    rcint *result = rc_alloc(sizeof(int), rc_type_int, NULL);
    return_value_unless(result, NULL);
    result->value = value;
    return result;
}

int rcint_value(rcint *ri) {
    return_value_unless(ri, 0);
    return ri->value;
}

int rcint_hash(rcint *ri) {
    return ri->value & INT_MAX;
}

rcstr *rcint2str(rcint *ri) {
    static char buffer[32];
    char *cur = buffer + sizeof(buffer);
    *--cur = 0;
    
    int value = rcint_value(ri);
    if (value < 0) {
        for (;;) {
            *--cur = (10 - (value % 10)) % 10 + '0';
            value /= 10;
            if (!value) break;
        }
        *--cur = '-';
    } else {
        for (;;) {
            *--cur = (value % 10) + '0';
            value /= 10;
            if (!value) break;
        }
    }
    
    return rcstr_dup(cur);
}
