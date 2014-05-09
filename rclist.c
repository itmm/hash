#include "rclist.h"

#include "log.h"
#include "rc.h"

#include <stdio.h>
#include <string.h>

struct rclist {
    void *key;
    void *value;
    void *next;
};


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

void *rclist_key(rclist *lst) {
    return_value_unless(lst, NULL);
    return lst->key;
}

void *rclist_value(rclist *lst) {
    return_value_unless(lst, NULL);
    return lst->value;
}

void *rclist_next(rclist *lst) {
    return_value_unless(lst, NULL);
    return lst->next;
}

rcstr *rclist2str(rclist *lst) {
    if (!lst) {
        return rcstr_dup("");
    }
    
    rcstr *key = rc2str(lst->key);
    rcstr *value = rc2str(lst->value);
    rcstr *next = rc2str(lst->next);
    
    if (key && lst->key && (rc_get_type(lst->key) == rc_type_list  || rc_get_type(lst->key) == rc_type_hash)) {
        rcstr *new_key = rcstr_dups(3, (const char *[]) { "(", rcstr_str(key), ")" });
        rc_release(key);
        key = new_key;
    }
    if (value && lst->value && (rc_get_type(lst->value) == rc_type_list  || rc_get_type(lst->value) == rc_type_hash)) {
        rcstr *new_value = rcstr_dups(3, (const char *[]) { "(", rcstr_str(value), ")" });
        rc_release(value);
        value = new_value;
    }
    if (next && lst->next) {
        if (rc_get_type(lst->next) == rc_type_list) {
            rcstr *new_next = rcstr_dups(2, (const char *[]) { " ", rcstr_str(next) });
            rc_release(next);
            next = new_next;
        } else {
            rcstr *new_next = rcstr_dups(2, (const char *[]) { " . ", rcstr_str(next) });
            rc_release(next);
            next = new_next;
        }
    }
    
    if (!key || !value || !next) {
        log_error("can't allocate all intermediate strings");
        rc_release(key);
        rc_release(value);
        rc_release(next);
        return NULL;
    }
    
    rcstr *result = rcstr_dups(4, (const char *[]) { rcstr_str(key), ":", rcstr_str(value), rcstr_str(next) });
    rc_release(key);
    rc_release(value);
    rc_release(next);

    return_value_unless(result, NULL);
    return result;
}

int rclist_hash(rclist *lst) {
    return_value_unless(lst, 0);
    
    return rc_hash(lst->key) + rc_hash(lst->value) * 0x1007 + rc_hash(lst->next) * 0x10000b;
}
