#include "rcint.h"

#include "log.h"
#include "rc.h"

void *rcint_alloc(int value) {
    int *result = rc_alloc(sizeof(int), rc_type_int, NULL);
    return_value_unless(result, NULL);
    *result = value;
    return result;
}
