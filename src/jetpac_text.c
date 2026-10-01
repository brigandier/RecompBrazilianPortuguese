/* Drawing setup adapted from Rainchus/dk64 src/jetpac/code_4FC0.c. */
#include "modding.h"
#include "ultra64.h"
#include "text_overrides.h"
extern u8 D_jetpac_8002E9C0,D_jetpac_8002E9C4,D_jetpac_8002E9C8,D_jetpac_8002E9CC;
extern u8 D_jetpac_80045BE0,D_jetpac_80045BE1,D_jetpac_80045BE2,D_jetpac_80045BE3;
extern Gfx *func_global_asm_806FD490(Gfx*,s32,s16,s16,char*);
RECOMP_PATCH
void func_jetpac_8002AEFC(Gfx **arg0, char *string, s32 x, s32 y, s32 arg4) {
    Gfx *dl;

    if (ptbr_text_equal((u8*)string,"5   START GAME")) string=PTBR_JETPAC_START;
    /* Pause options use eight-pixel cells, including the @ space. */
    if (ptbr_text_equal((u8*)string, PTBR_JETPAC_RETURN) ||
        ptbr_text_equal((u8*)string, PTBR_JETPAC_DELETE) ||
        ptbr_text_equal((u8*)string, PTBR_JETPAC_EXIT)) {
        s32 length = 0;
        while (string[length]) ++length;
        x = (320 - length * 8) / 2;
    }
    dl = *arg0;
    if (arg4 != 0) {
        gDPSetPrimColor(dl++, 0, 0, D_jetpac_8002E9C0, D_jetpac_8002E9C4, D_jetpac_8002E9C8, D_jetpac_8002E9CC);
        gDPSetEnvColor(dl++, D_jetpac_80045BE0, D_jetpac_80045BE1, D_jetpac_80045BE2, D_jetpac_80045BE3);
    } else {
        gDPSetPrimColor(dl++, 0, 0, D_jetpac_80045BE0, D_jetpac_80045BE1, D_jetpac_80045BE2, D_jetpac_80045BE3);
        gDPSetEnvColor(dl++, D_jetpac_8002E9C0, D_jetpac_8002E9C4, D_jetpac_8002E9C8, D_jetpac_8002E9CC);
    }
    *arg0 = func_global_asm_806FD490(dl, 4, x, y, string);
}
