#if !defined(rclist_h)
#define rclist_h

    #if !defined(rcstr_h)
        #include "rcstr.h"
    #endif

    typedef void *rclist;

    rclist rclist_cons(void *key, void *value, void *next);

    void *rclist_key(rclist lst);
    void *rclist_value(rclist lst);
    void *rclist_next(rclist lst);

    rcstr rclist2str(rclist lst);
    int rclist_hash(rclist lst);

#endif
