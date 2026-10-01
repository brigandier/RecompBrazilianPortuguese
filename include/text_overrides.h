#ifndef PTBR_TEXT_OVERRIDES_H
#define PTBR_TEXT_OVERRIDES_H
#include "extra_texts.h"
/* Latin-1 strings, shared by runtime code and native mapping tests. */
static int ptbr_text_equal(const unsigned char *a, const char *b) {
    if (!a) return 0;
    while (*a && *a == (unsigned char)*b) { ++a; ++b; }
    return *a == (unsigned char)*b;
}
#endif
