#if !defined(rcstr_h)
#define rcstr_h

    typedef const char *rcstr;

    rcstr rcstr_dup(const char *src);
    rcstr rcstr_dups(const char *first, ...);

    rcstr rc2str(void *rc);

#endif
