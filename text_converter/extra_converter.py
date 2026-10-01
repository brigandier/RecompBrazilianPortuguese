"""Generate supplementary translation headers from UTF-8 text files."""
import ast
import math
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
KEYS = set("""
GAME_OVER TIME_OUT BAD_LUCK SCORE ARCADE_NAME ARCADE_TIME ARCADE_NAME_WAS
ARCADE_REGISTERED ARCADE_REGISTRATION ARCADE_RECORD ARCADE_GAME_OVER
ARCADE_RETURN ARCADE_DELETE ARCADE_RANK JETPAC_MENU JETPAC_ONE_PLAYER
JETPAC_TWO_PLAYERS JETPAC_KEYBOARD JETPAC_CONTROLLER JETPAC_RETURN
JETPAC_DELETE JETPAC_EXIT JETPAC_COIN JETPAC_GAME_OVER WELL_DONE LAP_BONUS
ARCADE_CREDITS ARCADE_EXIT ARCADE_HEIGHT ARCADE_INSERT ARCADE_PLAYER
ARCADE_COIN JETPAC_START SPEAKER_KROOL SPEAKER_KLUMP
""".split())


def c_string(text):
    data = text.encode("latin-1")
    if any(c < 32 or c == 127 for c in data):
        raise ValueError("Control characters are not allowed in translation text")
    return '"' + ''.join(chr(c) if 32 <= c < 127 and c not in (34, 92)
                         else f"\\{c:03o}" for c in data) + '"'


def read_messages(path):
    result = {}
    for number, line in enumerate(path.read_text(encoding="utf-8-sig").splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        key, separator, text = line.partition(" = ")
        if not separator or key not in KEYS or key in result or not text:
            raise ValueError(f"{path.name}:{number}: invalid, duplicate or empty entry")
        c_string(text)
        result[key] = text
    if result.keys() != KEYS:
        raise ValueError(f"Missing entries: {sorted(KEYS - result.keys())}")
    return result


def validate_messages(root, messages):
    # These slots are part of the original game and have fixed byte capacities.
    source = (root / "src/fixed_text.c").read_text()
    slots = re.findall(r'PTBR_SLOT\([^\n]+?, ("(?:[^"\\]|\\.)*"), PTBR_(\w+), (\d+)\);', source)
    for original, key, capacity in slots:
        text = messages[key]
        if len(text.encode("latin-1")) + 1 > int(capacity):
            raise ValueError(f"{key}: maximum {int(capacity) - 1} bytes")
        tokens = r'%(?:[-+0 #]*\d*(?:\.\d+)?[a-zA-Z%])'
        if re.findall(tokens, text) != re.findall(tokens, ast.literal_eval(original)):
            raise ValueError(f"{key}: preserve the original printf placeholders")
    # The Arcade renderer supports its original alphabet plus these accents.
    arcade_alphabet = set("ABCDEFGHIJKLMNOPQRSTUVWXYZ.-?{}:=0123456789<>m)!@#$%^&cab ÉÍÊ")
    for key in ("ARCADE_CREDITS", "ARCADE_EXIT", "ARCADE_HEIGHT", "ARCADE_INSERT",
                "ARCADE_PLAYER", "ARCADE_COIN"):
        if set(messages[key]) - arcade_alphabet:
            raise ValueError(f"{key}: unsupported Arcade character")
        limit = 8 if key in ("ARCADE_PLAYER", "ARCADE_COIN") else 36
        if len(messages[key]) > limit:
            raise ValueError(f"{key}: maximum {limit} characters for this layout")


def font_widths(root):
    source = (root / "src/font.c").read_text()
    chars = re.search(r'fontstring_6\[\] = \{(.*?)\};', source, re.S)[1]
    metrics = re.search(r'font_6_characters\[\] = \{(.*?)\};', source, re.S)[1]
    codes = [int(x) for x in re.findall(r'\d+', chars)][:-1]
    widths = [int(x) + 1 for x in re.findall(r'\{\s*\d+,\s*(\d+)\s*\}', metrics)]
    return dict(zip(map(chr, codes), widths)) | {" ": 6}


def check_line(text, widths):
    c_string(text)
    if set(text) - widths.keys():
        raise ValueError(f"Unsupported subtitle characters: {set(text) - widths.keys()}")
    if sum(widths[c] for c in text) * 3 > (320 - 48) * 4:
        raise ValueError(f"Subtitle line is too wide; split or shorten it: {text}")


def read_cues(path, widths):
    cues = []
    previous_end = 0
    for number, line in enumerate(path.read_text(encoding="utf-8-sig").splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        fields = line.split("|")
        if len(fields) != 5:
            raise ValueError(f"{path.name}:{number}: expected five fields separated by |")
        start, end, speaker, first, second = fields
        start, end = float(start), float(end)
        if not (math.isfinite(start) and math.isfinite(end) and previous_end <= start < end):
            raise ValueError(f"{path.name}:{number}: invalid order, overlap or duration")
        if speaker not in ("KROOL", "KLUMP") or not first:
            raise ValueError(f"{path.name}:{number}: unknown speaker or empty first line")
        check_line(first, widths)
        check_line(second, widths)
        cues.append((start, end, speaker, first, second))
        previous_end = end
    if not cues:
        raise ValueError("At least one subtitle cue is required")
    return cues


def generate(root=ROOT):
    messages = read_messages(root / "text_files/[43] - Extra Text.txt")
    validate_messages(root, messages)
    widths = font_widths(root)
    for key in ("SPEAKER_KROOL", "SPEAKER_KLUMP"):
        check_line(messages[key], widths)
    cues = read_cues(root / "text_files/[44] - Intro Subtitles.txt", widths)
    extra = "/* Generated by text_converter/extra_converter.py. */\n#ifndef PTBR_EXTRA_TEXTS_H\n#define PTBR_EXTRA_TEXTS_H\n"
    extra += ''.join(f"#define PTBR_{key} {c_string(text)}\n" for key, text in messages.items())
    extra += "#endif\n"
    intro = "/* Generated by text_converter/extra_converter.py. */\nstatic const IntroCue cues[] = {\n"
    for start, end, speaker, first, second in cues:
        intro += f"    {{{start!r}f,{end!r}f,(const u8*){c_string(first)},(const u8*){c_string(second)},SPEAKER_{speaker}}},\n"
    intro += "};\n"
    # Validate both inputs before replacing either generated file.
    (root / "include/extra_texts.h").write_text(extra, encoding="ascii")
    (root / "include/intro_cues.h").write_text(intro, encoding="ascii")
    print(f"Generated {len(messages)} supplementary messages and {len(cues)} subtitle cues")


if __name__ == "__main__":
    generate()
