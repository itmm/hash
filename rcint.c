#include "rcint.h"

#include "log.h"
#include "rc.h"

#include <stdbool.h>

void *rcint_alloc(int value) {
    int *result = rc_alloc(sizeof(int), rc_type_int, NULL);
    return_value_unless(result, NULL);
    *result = value;
    return result;
}

rcstr *rcint2str(void *rcint) {
    static char buffer[32];
    char *cur = buffer + sizeof(buffer);
    *--cur = 0;
    
    int value = *((int *) rcint);
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
