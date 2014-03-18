#if !defined(rcstr_h)
#define rcstr_h

    #include <stdlib.h>
    
    typedef void *rcstr;

    rcstr rcstr_dup(const char *src);
    rcstr rcstr_dups(size_t count, const char *srcs[count]);

    rcstr rc2str(void *rc);

#endif
