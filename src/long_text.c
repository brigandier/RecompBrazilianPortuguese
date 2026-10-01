/* Adapted from Rainchus/dk64 src/global_asm/code_1025E0.c.
 * Substitute before allocating the animated text and per-character state.
 * Longer translations never overwrite the original ROM string slots.
 */
#include "modding.h"
#include "ultra64.h"
#include "text_overrides.h"
typedef struct { f32 unk0,unk4,unk8,unkC,unk10,unk14,unk18,unk1C,unk20,unk24,unk28,unk2C,unk30,unk34; } Struct806FD9FC;
typedef struct Struct80754AD0 {
 struct Struct80754AD0 *next; u8 *unk4; Struct806FD9FC *unk8;
 u8 unkC,unkD,padE[2]; f32 unk10,unk14,unk18; s16 unk1C;
} Struct80754AD0;
_Static_assert(sizeof(Struct80754AD0)==0x20,"text object layout");
_Static_assert(sizeof(Struct806FD9FC)==0x38,"character object layout");
extern Struct80754AD0 *D_global_asm_80754AD0;
extern s16 D_global_asm_80754AD4;
extern u8 D_global_asm_8074450C;
extern s32 getCenterOfString(s16,u8*);
extern void *_malloc(s32);
extern void func_global_asm_80611690(void*);
extern void _strcpy(u8*,u8*);
extern s16 func_global_asm_806FBEAC(u8*);
extern s32 rand(void);
static u32 ptbr_strlen(const u8 *s) { u32 n=0;while(s[n])++n;return n; }
RECOMP_PATCH
s16 func_global_asm_806FDB8C(s16 arg0, u8 *arg1, u8 arg2, f32 arg3, f32 arg4, f32 arg5) {
    Struct80754AD0 *temp_v0;
    Struct80754AD0 *var_v1;
    s16 i;
    u32 len;

    if (ptbr_text_equal(arg1, "WELL DONE!")) {
        arg1 = (u8*)PTBR_WELL_DONE;
    } else if (ptbr_text_equal(arg1, "LAP BONUS")) {
        arg1 = (u8*)PTBR_LAP_BONUS;
    }
    getCenterOfString(arg0, arg1);
    temp_v0 = _malloc(sizeof(Struct80754AD0));
    func_global_asm_80611690(temp_v0);
    if (D_global_asm_80754AD0 == NULL) {
        D_global_asm_80754AD0 = temp_v0;
        D_global_asm_80754AD0->next = NULL;
        D_global_asm_80754AD4 = 0;
    } else {
        var_v1 = D_global_asm_80754AD0;
        while (var_v1->next) {
            var_v1 = var_v1->next;
        }
        var_v1->next = temp_v0;
        temp_v0->next = NULL;
    }
    temp_v0->unk1C = D_global_asm_80754AD4++;
    len = ptbr_strlen(arg1);
    temp_v0->unk4 = _malloc(len + 1);
    _strcpy(temp_v0->unk4, arg1);
    temp_v0->unkD = func_global_asm_806FBEAC(arg1);
    temp_v0->unk8 = _malloc(temp_v0->unkD * sizeof(Struct806FD9FC));
    func_global_asm_80611690(temp_v0->unk8);
    temp_v0->unkC = arg0;
    for (i = 0; i < temp_v0->unkD; i++) {
        if (arg2 == 3) {
            temp_v0->unk8[i].unk0 = (((rand() >> 0xF) % 1440) - 0x50);
            temp_v0->unk8[i].unk4 = (((rand() >> 0xF) % 1120) - 0x50);
            temp_v0->unk8[i].unk20 = (((rand() >> 0xF) % 6283) / 1000.0);
            temp_v0->unk14 = 0.1f;
        } else {
            temp_v0->unk8[i].unk0 = arg3 * 4.0f;
            temp_v0->unk8[i].unk4 = arg4 * 4.0f;
            temp_v0->unk8[i].unk8 = arg5 * 4.0f;
            temp_v0->unk8[i].unk30 = D_global_asm_8074450C == 2 ? 1.0 : 0.1;
            temp_v0->unk14 = 0.2f;
        }
    }
    return temp_v0->unk1C;
}
