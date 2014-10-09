#include "rcstr.h"

#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "rc.h"
#include "rclist.h"
#include "rcint.h"

struct rcstr {};

rcstr *rcstr_dup(const char *src) {
    return rcstr_dups(1, (const char *[]) { src });
}

rcstr *rcstr_dups(size_t count, const char *srcs[count]) {
    size_t length = 1;
    const char **begin = srcs;
    const char **end = begin + count;
    for (const char **cur = begin; cur != end; ++cur) {
        if (*cur) { length += strlen(*cur); }
    }

    void *result = rc_alloc(length, rc_type_string, NULL);
    return_unless(NULL, result);

    char *dest = result;
    for (const char **cur = begin; cur != end; ++cur) {
        if (*cur) {
            size_t len = strlen(*cur);
            memcpy(dest, *cur, len);
            dest += len;
        }
    }
    *dest = 0;
    return result;
}

bool has_matching_closing_quote(const char *cur, const char *end) {
    int balance = 0;
    for (; cur != end; ++cur) {
        switch (*cur) {
            case '[': ++balance; break;
            case ']':
                if (balance) { --balance; }
                else { return true; }
        }
    }
    return false;
}

bool has_matching_opening_quote(const char *cur, const char *prebegin) {
    int balance = 0;
    for (; cur != prebegin; --cur) {
        switch (*cur) {
            case ']': ++balance; break;
            case '[':
                if (balance) { --balance; }
                else { return true; }
        }
    }
    return false;
}

rcstr *rcstr2str(rcstr *str) {
    if (!str) { return rcstr_dup(""); }
    
    const char *cur = (const char *) str;
    const char *end = cur + strlen(cur);
    
    char *out_begin = NULL;
    char *out = NULL;
    char *out_end = NULL;
    
    bool needs_escape = false;
    for (; cur != end; ++cur) {
        bool escape_char = false;
        char ch = *cur;
        if (ch <= ' ') { needs_escape = true; }
        else switch (ch) {
            case '(':
            case ')':
                needs_escape = true;
                break;
            case '[':
                if (has_matching_closing_quote(cur + 1, end)) {
                    needs_escape = true;
                } else {
                    escape_char = true;
                }
                break;
            case ']':
                if (has_matching_opening_quote(cur - 1, (const char *) str - 1)) {
                    needs_escape = true;
                } else {
                    escape_char = true;
                }
                break;
            case ':':
                needs_escape = true;
                break;
            case '\\':
                escape_char = true;
            default:
                break;
        }
        
        if (out && out_end - out < 3) {  // backslash + char + 0
            size_t used = out - out_begin;
            size_t needed = used * 2;
            out_begin = realloc(out_begin, needed);
            return_unless(NULL, out_begin);
            out = out_begin + used;
            out_end = out_begin + needed;
        }
        if (escape_char) {
            needs_escape = true;
            if (!out) {
                size_t used = cur - (const char *) str;
                size_t needed = used + 10;
                out_begin = malloc(needed);
                return_unless(NULL, out_begin);
                out_end = out_begin + needed;
                memcpy(out_begin, str, used);
                out = out_begin + used;
            }
            *out++ = '\\';
        }
        if (out) { *out++ = ch; };
    }
    
    rcstr *result;
    if (out) {
        *out = 0;
        result = rcstr_dups(3, (const char *[]) { "[", out_begin, "]" });
        free(out_begin);
    } else if (needs_escape) {
        result = rcstr_dups(3, (const char *[]) { "[", (const char *) str, "]" } );
    } else {
        result = rc_retain(str);
    }
    return_unless(NULL, result);
    return result;
}

const char *rcstr_str(rcstr *rs) {
    return_unless(NULL, rs);
    return (const char *) rs;
}

rcstr *rc2str(void *rc) {
    if (!rc) { return rcstr_dup(""); }
    switch (rc_get_type(rc)) {
        case rc_type_string: return rcstr2str(rc);
        case rc_type_list: return rclist2str(rc);
        case rc_type_int: return rcint2str(rc);
        case rc_type_true: return rcstr_dup("true");
        case rc_type_false: return rcstr_dup("false");
        default:
            log_error("can't stringify type %d", (int) rc_get_type(rc));
            return rcstr_dup("???");
    }
}

int rcstr_hash(rcstr *rs) {
    const char *cur = rcstr_str(rs);
    return_unless(0, cur);
    int result = 0;
    for (; *cur; ++cur) {
        result = result * 17 + *cur;
    }
    return result & INT_MAX;
}

