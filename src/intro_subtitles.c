/* Opening cutscene subtitles.
 * Overlay dispatch adapted from Rainchus/dk64 code_8F4B0.c.
 * Retain the original callbacks and append subtitles at the last overlay pass.
 */
#ifndef F3DEX_GBI_2
#define F3DEX_GBI_2
#endif
#include "modding.h"
#include "recomputils.h"
#include "ultra64.h"
#include "font.h"
#include "extra_texts.h"
#if defined(G_SETOTHERMODE_H) && (G_SETOTHERMODE_H != 0xE3 || G_RDPHALF_1 != 0xE1)
#error Subtitle renderer requires the DK64 F3DEX2 command encoding
#endif

typedef struct {
    Gfx *(*draw)(Gfx *, void *);
    void *actor;
    u8 pass;
    s8 padding[3];
} SubtitleOverlay;
_Static_assert(sizeof(SubtitleOverlay) == 12, "original overlay layout");
extern SubtitleOverlay D_global_asm_807FC630[];
extern u8 D_global_asm_807501E0, is_cutscene_active;
extern s32 current_map;
extern s16 D_global_asm_80744490, D_global_asm_80744494;
extern u8 isIntroStoryPlaying(void);
extern s16 D_global_asm_807444B0, D_global_asm_807444AC;
extern CharStruct font_6_characters[];
extern u8 fontstring_6[], fontstarts_6[];
extern s8 fontydeltas_6[];
extern Gfx *func_global_asm_806FBEF0(Gfx *dl, u8 font, s16 page);

static int glyph_index(u8 c) {
    int i;
    for (i=0; fontstring_6[i]; ++i) if (fontstring_6[i]==c) return i;
    return -1;
}

RECOMP_CALLBACK("*", recomp_adjust_dl_allocation)
void ptbr_subtitle_dl_budget(s32 *commands) { *commands += 2048; }

enum { SPEAKER_KROOL, SPEAKER_KLUMP };
typedef struct { f32 start,end; const u8 *first,*second; u8 speaker; } IntroCue;
#include "intro_cues.h"
static f32 intro_seconds;
static int clock_running, current_cue=-1;
static u32 last_clock;
static int intro_map(void) {
    return current_map==152 || current_map==153 || current_map==171 || current_map==172;
}
static int visible(void) {
    return intro_map() && isIntroStoryPlaying() && is_cutscene_active != 0 && is_cutscene_active != 6;
}

RECOMP_CALLBACK("*", dk64recomp_every_frame)
void ptbr_intro_update(void) {
    u32 now=(u32)osGetTime();
    int i, next=-1;
    if(!intro_map() || !isIntroStoryPlaying()) {
        clock_running=0;intro_seconds=0;current_cue=-1;last_clock=now;return;
    }
    if(!clock_running) {clock_running=1;intro_seconds=0;last_clock=now;}
    /* N64 OS clock: 46,875,000 ticks/s, independent of render FPS.
     * Unsigned subtraction handles wrap. */
    f32 delta=(f32)(u32)(now-last_clock)/46875000.0f;
    last_clock=now;
    if(is_cutscene_active!=6 && delta<2.0f) intro_seconds+=delta;
    if(visible()) for(i=0;i<(int)(sizeof(cues)/sizeof(cues[0]));++i)
        if(intro_seconds>=cues[i].start && intro_seconds<cues[i].end) {next=i;break;}
    current_cue=next;
}

/* Draw the unchanged IA8 font through the game's F3DEX2 command format.
 * Texture filtering preserves fractional alpha and subpixel glyph positions.
 */
static Gfx *draw_line(Gfx *dl, const u8 *text, int line, int lines) {
    int width=D_global_asm_80744490, height=D_global_asm_80744494;
    int bottom=D_global_asm_807444B0;
    int top=D_global_asm_807444AC;
    int name_line=(line<0);
    int i,n,page,loaded=-1,total=0,pen=0;
    if(width<160 || height<120 || bottom<0 || bottom>height-(lines*14+6)) return dl;
    for(i=0;text[i];++i) {n=glyph_index(text[i]);total+=n<0?6:font_6_characters[n].width+1;}
    /* Quarter-pixel geometry: three units per original pixel = 75% size. */
    int x=24*4;
    int y=(name_line ? top-18 : bottom+5+line*14)*4;
    if(total*3>(width-48)*4 || (name_line && top<22)) return dl;
    gDPPipeSync(dl++);
    gDPSetScissor(dl++,G_SC_NON_INTERLACE,0,name_line?0:bottom,width,name_line?top:height);
    gDPSetCycleType(dl++,G_CYC_1CYCLE);
    gDPSetTexturePersp(dl++,G_TP_NONE);
    gDPSetTextureLOD(dl++,G_TL_TILE);
    gDPSetTextureDetail(dl++,G_TD_CLAMP);
    gDPSetTextureLUT(dl++,G_TT_NONE);
    gDPSetTextureConvert(dl++,G_TC_FILT);
    gDPSetTextureFilter(dl++,G_TF_BILERP);
    gDPSetAlphaCompare(dl++,G_AC_NONE);
    gDPSetCombineMode(dl++,G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
    /* Speaker colors: orange for Klump, green for K. Rool. */
    if(cues[current_cue].speaker==SPEAKER_KLUMP) {
        gDPSetPrimColor(dl++,0,0,255,176,64,255);
    } else {
        gDPSetPrimColor(dl++,0,0,128,255,64,255);
    }
    for(i=0;text[i];++i) {
        n=glyph_index(text[i]);
        if(n<0){pen+=6;continue;}
        page=0;
        while(page+1<FONT_6_COUNT && n>=fontstarts_6[page+1]) ++page;
        if(page!=loaded) {
            gDPPipeSync(dl++);
            /* Use the engine font cache: mod data lives outside the normal
             * segmented texture range and must not be passed as a raw address. */
            dl=func_global_asm_806FBEF0(dl,6,page+1);
            loaded=page;
        }
        int gx=x+pen*3,gy=y+fontydeltas_6[n]*3;
        int w=font_6_characters[n].width;
        gSPTextureRectangle(dl++,gx,gy,gx+w*3,gy+FONT_6_HEIGHT*3,
            G_TX_RENDERTILE,font_6_characters[n].x_start*32,0,1365,1365);
        pen+=w+1;
    }
    gDPPipeSync(dl++);
    gDPSetTexturePersp(dl++,G_TP_PERSP);
    gDPSetScissor(dl++,G_SC_NON_INTERLACE,0,0,width,height);
    return dl;
}

static Gfx *draw_subtitles(Gfx *dl) {
    if(!visible() || current_cue<0) return dl;
    const IntroCue *cue=&cues[current_cue];
    int lines=cue->second[0] ? 2 : 1;
    dl=draw_line(dl,(const u8*)(cue->speaker==SPEAKER_KROOL?PTBR_SPEAKER_KROOL:PTBR_SPEAKER_KLUMP),-1,1);
    dl=draw_line(dl,cue->first,0,lines);
    if(lines==2) dl=draw_line(dl,cue->second,1,lines);
    return dl;
}

RECOMP_PATCH Gfx *func_global_asm_8068C20C(Gfx *dl, u8 pass) {
    u8 i = D_global_asm_807501E0;
    while (i) {
        --i;
        if (pass == D_global_asm_807FC630[i].pass) {
            gDPPipeSync(dl++);
            dl = D_global_asm_807FC630[i].draw(dl, D_global_asm_807FC630[i].actor);
        }
    }
    if (pass == 7) {
        D_global_asm_807501E0 = 0;
        dl = draw_subtitles(dl);
    }
    return dl;
}
