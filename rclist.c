#include "rclist.h"

#include "log.h"
#include "rc.h"

static void _rclist_dealloc(void *rc) {
    return_unless(rc);
    return_unless(rc_get_type(rc) == rc_type_list);
    
    rclist *list = rc;
    rc_release(list->key);
    rc_release(list->value);
    rc_release(list->next);
}

rclist *rclist_cons(void *key, void *value, void *next) {
    rclist *result = rc_alloc(sizeof(rclist), rc_type_list, _rclist_dealloc);
    return_value_unless(result, NULL);
    
    result->key = rc_retain(key);
    result->value = rc_retain(value);
    result->next = rc_retain(next);
    
    return result;
}
