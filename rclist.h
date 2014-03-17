#if !defined(rclist_h)
#define rclist_h

    typedef struct {
        void *key;
        void *value;
        void *next;
    } rclist;

    rclist *rclist_cons(void *key, void *value, void *next);

#endif
