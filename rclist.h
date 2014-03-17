#if !defined(rclist_h)
#define rclist_h

    typedef struct {
        void *head;
        void *tail;
    } rclist;

    rclist *rclist_cons(void *head, void *tail);

#endif
