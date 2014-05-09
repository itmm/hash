#if !defined(rcint_h)
#define rcint_h

    #if !defined(rcstr_h)
        #include "rcstr.h"
    #endif

    typedef struct rcint rcint;

    rcint *rcint_alloc(int value);

    int rcint_value(rcint *ri);

    int rcint_hash(rcint *ri);
    rcstr *rcint2str(rcint *ri);

#endif
