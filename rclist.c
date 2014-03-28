#include "rclist.h"

#include "log.h"
#include "rc.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    void *key;
    void *value;
    void *next;
} rclist_internal;


static void _rclist_dealloc(rclist rc) {
    return_unless(rc);
    return_unless(rc_get_type(rc) == rc_type_list);
    
    rclist_internal *list = rc;
    rc_release(list->key);
    rc_release(list->value);
    rc_release(list->next);
}

rclist rclist_cons(void *key, void *value, void *next) {
    rclist_internal *result = rc_alloc(sizeof(rclist_internal), rc_type_list, _rclist_dealloc);
    return_value_unless(result, NULL);
    
    result->key = rc_retain(key);
    result->value = rc_retain(value);
    result->next = rc_retain(next);
    
    return result;
}

void *rclist_key(rclist lst) {
    return_value_unless(lst, NULL);
    return ((rclist_internal *) lst)->key;
}

void *rclist_value(rclist lst) {
    return_value_unless(lst, NULL);
    return ((rclist_internal *) lst)->value;
}

void *rclist_next(rclist lst) {
    return_value_unless(lst, NULL);
    return ((rclist_internal *) lst)->next;
}

rcstr rclist2str(rclist lst) {
    if (!lst) {
        return rcstr_dup("");
    }
    
    rclist_internal *real = lst;
    rcstr key = rc2str(real->key);
    rcstr value = rc2str(real->value);
    rcstr next = rc2str(real->next);
    
    if (key && real->key && (rc_get_type(real->key) == rc_type_list  || rc_get_type(real->key) == rc_type_hash)) {
        rcstr new_key = rcstr_dups(3, (const char *[]) { "(", key, ")" });
        rc_release((void *) key);
        key = new_key;
    }
    if (value && real->value && (rc_get_type(real->value) == rc_type_list  || rc_get_type(real->value) == rc_type_hash)) {
        rcstr new_value = rcstr_dups(3, (const char *[]) { "(", value, ")" });
        rc_release((void *) value);
        value = new_value;
    }
    if (next && real->next) {
        if (rc_get_type(real->next) == rc_type_list) {
            rcstr new_next = rcstr_dups(2, (const char *[]) { " ", next });
            rc_release((void *) next);
            next = new_next;
        } else {
            rcstr new_next = rcstr_dups(2, (const char *[]) { " . ", next });
            rc_release((void *) next);
            next = new_next;
        }
    }
    
    if (!key || !value || !next) {
        log_error("can't allocate all intermediate strings");
        rc_release((void *) key);
        rc_release((void *) value);
        rc_release((void *) next);
        return NULL;
    }
    
    rcstr result = rcstr_dups(4, (const char *[]) { key, ":", value, next });
    rc_release((void *) key);
    rc_release((void *) value);
    rc_release((void *) next);

    return_value_unless(result, NULL);
    return result;
}

int rclist_hash(rclist lst) {
    return_value_unless(lst, 0);
    
    rclist_internal *real = lst;
    return rc_hash(real->key) + rc_hash(real->value) * 0x1007 + rc_hash(real->next) * 0x10000b;
}
