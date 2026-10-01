#ifndef PTBR_TEXT_OVERRIDES_H
#define PTBR_TEXT_OVERRIDES_H
#include "extra_texts.h"
#define PTBR_ARCADE_COLUMNS PTBR_ARCADE_PLAYER "    " PTBR_ARCADE_COIN
/* Latin-1 strings, shared by runtime code and native mapping tests. */
static int ptbr_text_equal(const unsigned char *a, const char *b) {
    if (!a) return 0;
    while (*a && *a == (unsigned char)*b) { ++a; ++b; }
    return *a == (unsigned char)*b;
}
static unsigned char *ptbr_long_message(unsigned char *text) {
    if (ptbr_text_equal(text,"WELL DONE!"))
        return (unsigned char*)PTBR_WELL_DONE;
    if (ptbr_text_equal(text,"LAP BONUS"))
        return (unsigned char*)PTBR_LAP_BONUS;
    return text;
}
static unsigned char *ptbr_arcade_message(unsigned char *text) {
    if (ptbr_text_equal(text,"CREDIT 00")) return (unsigned char*)PTBR_ARCADE_CREDITS;
    if (ptbr_text_equal(text,"EXIT DK ARCADE")) return (unsigned char*)PTBR_ARCADE_EXIT;
    if (ptbr_text_equal(text,"HOW HIGH CAN YOU GET ?")) return (unsigned char*)PTBR_ARCADE_HEIGHT;
    if (ptbr_text_equal(text,"INSERT COIN")) return (unsigned char*)PTBR_ARCADE_INSERT;
    if (ptbr_text_equal(text,"PLAYER    COIN")) return (unsigned char*)PTBR_ARCADE_COLUMNS;
    return text;
}
#endif
