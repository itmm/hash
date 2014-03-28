#if !defined(rcint_h)
#define rcint_h

    #if !defined(rcstr_h)
        #include "rcstr.h"
    #endif

    void *rcint_alloc(int value);

    rcstr *rcint2str(void *rcint);

#endif
