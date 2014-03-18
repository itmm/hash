#if !defined(rclist_h)
#define rclist_h

    #if !defined(rcstr_h)
        #include "rcstr.h"
    #endif

    typedef struct {
        void *key;
        void *value;
        void *next;
    } rclist;

    rclist *rclist_cons(void *key, void *value, void *next);

    rcstr rclist2str(rclist *lst);

#endif
