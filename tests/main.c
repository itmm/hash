
#include <stdlib.h>

#include "log.h"
#include "t_log.h"
#include "t_rc.h"
#include "t_rclist.h"
#include "t_rcstr.h"
#include "t_rcint.h"
#include "t_unit.h"
#include "unit.h"

#pragma mark - suite

int main(int argc, const char * argv[])
{
    unit_test *suite = test_suite_alloc(5, (unit_test *[]) {
        create_log_tests(),
        create_rc_tests(),
        create_rclist_tests(),
        create_rcstr_tests(),
        create_rcint_tests(),
        create_unit_tests(),
    });
    
    if (!suite) {
        log_error("can't alloc suite");
        exit(EXIT_FAILURE);
    }
    unit_state state = {0};
    test_run(suite, &state);
    test_summary(&state);
    test_free(suite);
    
    exit(state.failed ? EXIT_FAILURE : EXIT_SUCCESS);
}
