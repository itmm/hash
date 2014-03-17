#include "rclist.h"

#include "log.h"
#include "rc.h"

static void _rclist_dealloc(void *rc) {
    return_unless(rc);
    return_unless(rc_get_type(rc) == rc_type_list);
    rclist *list = rc;
    if (list->head) { rc_release(list->head); }
    if (list->tail) { rc_release(list->tail); }
}

rclist *rclist_cons(void *head, void *tail) {
    rclist *result = rc_alloc(sizeof(rclist), rc_type_list, _rclist_dealloc);
    return_value_unless(result, NULL);
    result->head = head;
    if (head) { rc_retain(head); }
    result->tail = tail;
    if (tail) { rc_retain(tail); }
    return result;
}
