/* Original Arcade drawing setup: Rainchus/dk64 src/arcade/code_0.c.
 * Keep the original 8-pixel Arcade letters. Added accent marks are small
 * pixel strokes above those letters, using the same text color.
 */
#include "modding.h"
#include "ultra64.h"
#include "text_overrides.h"
extern u8 arcade_text_red, arcade_text_green, arcade_text_blue, arcade_text_alpha;
extern s32 arcade_text_x, arcade_text_y;
extern Gfx *func_global_asm_806FD490(Gfx*, s32, s16, s16, char*);

RECOMP_PATCH void arcade_draw_text(Gfx **gpp, char *string) {
    Gfx *gp = *gpp;
    unsigned char *translated = ptbr_arcade_message((unsigned char*)string);
    char base[64];
    unsigned char accents[64];
    unsigned int i, count = 0;
    s16 x = (s16)arcade_text_x, y = (s16)arcade_text_y;
    int revised = translated != (unsigned char*)string;
    int columns = ptbr_text_equal(translated,PTBR_ARCADE_COLUMNS);
    int has_accents = 0;
    gDPPipeSync(gp++);
    gDPSetCycleType(gp++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gp++, -1);
    gSPSetGeometryMode(gp++, G_SHADE | G_SHADING_SMOOTH);
    gDPSetPrimColor(gp++, 0, 0, arcade_text_red, arcade_text_green, arcade_text_blue, arcade_text_alpha);
    gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    /* The original splits this sentence at fixed x positions. Leave one
     * 8-pixel space after the 12-character Portuguese first fragment. */
    if (x == 176 && y == 123 &&
        ptbr_text_equal((unsigned char*)string,PTBR_ARCADE_REGISTERED))
        x = 168;
    if (!revised) {
        *gpp = func_global_asm_806FD490(gp,2,x,y,string);
        return;
    }
    if (columns) {
        /* Center the headers over the existing 1 / 1 numeric columns. */
        gp = func_global_asm_806FD490(gp,2,96,y,PTBR_ARCADE_PLAYER);
        *gpp = func_global_asm_806FD490(gp,2,176,y,PTBR_ARCADE_COIN);
        return;
    }
    while (translated[count] && count < sizeof(base)-1) {
        unsigned char c = translated[count];
        accents[count]=0;
        if (c==0xC9) { c='E'; accents[count]=1; }
        else if (c==0xCD) { c='I'; accents[count]=1; }
        else if (c==0xCA) { c='E'; accents[count]=2; }
        has_accents |= accents[count];
        base[count++]=(char)c;
    }
    base[count]=0;
    if (ptbr_text_equal(translated,PTBR_ARCADE_INSERT) ||
        ptbr_text_equal(translated,PTBR_ARCADE_EXIT) ||
        ptbr_text_equal(translated,PTBR_ARCADE_HEIGHT))
        x=(320-(s16)count*8)/2;
    gp=func_global_asm_806FD490(gp,2,x,y,base);
    if (has_accents && arcade_text_alpha) {
        u32 color=((arcade_text_red>>3)<<11)|((arcade_text_green>>3)<<6)|((arcade_text_blue>>3)<<1)|1;
        gDPPipeSync(gp++);
        gDPSetCycleType(gp++,G_CYC_FILL);
        gDPSetFillColor(gp++,(color<<16)|color);
        for (i=0;i<count;++i) {
            s16 px=x+(s16)i*8;
            if (accents[i]==1) {
                gDPFillRectangle(gp++,px+4,y-3,px+5,y-3);
                gDPFillRectangle(gp++,px+3,y-2,px+4,y-2);
            } else if (accents[i]==2) {
                gDPFillRectangle(gp++,px+3,y-3,px+4,y-3);
                gDPFillRectangle(gp++,px+2,y-2,px+2,y-2);
                gDPFillRectangle(gp++,px+5,y-2,px+5,y-2);
            }
        }
        gDPPipeSync(gp++);
        gDPSetCycleType(gp++,G_CYC_1CYCLE);
    }
    *gpp=gp;
}
