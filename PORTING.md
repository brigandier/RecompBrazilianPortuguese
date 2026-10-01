
# Porting this over to your own language

Here is a step by step guide on how to make your own language mod with this framework:
## Extracting Vanilla Text
1.  To do so, place your US (`dk64.z64`) or PAL (`dk64_pal.z64`) z64 ROM into `/text_converter`
2. Run either `text_decoder.py` or `text_decoder_pal.py` depending on what version your ROM is. 
> [!NOTE] 
> The PAL extractor will automatically extract the English text from ROM. If you wish to extract the other languages, change the 2nd arg of `grabText` in that file to 1 (French), 2 (German) or 3 (Spanish).
3. This will write a bunch of files in `/text_files/jsonstorage`. To convert these to the `.txt` files, go to `/text_files` and run `to_txt.py`

## Changing the text
1. To change the text, just overwrite the text files you wish to change. See the Text Syntax section for how these files are written
2. Once you are happy with the changes you've made, go to `/text_converter` and run `converter.py`. This will write the text files to a format that DK64 can understand better to `/src/text.c`
3. Run `make` and `./build.sh` or `./build.bat` (depending on your OS) to build your changes into the `.nrm`

## Adding new font characters
The font is stored in `/font` and will only currently modify the "white font" (what's seen in text bubbles, K Rool story intro text etc) and the "yellow font" (What's seen on the pause menu, level intro banners etc).
The white font is actually based on (or just heavily similar to) the `Tekton Bold` font. As such, if possible, try and add your new characters via what that character looks like with `Tekton Bold`. The yellow font unfortunately doesn't have an accompanying font type, so you'll have to get creative with this one.
1. To add a new font character, add the image of that character to the accompanying font directory (`/font/white` for the white font, `/font/yellow` for the yellow font). It must be the `.png` format, and the height must match what is expected for that font (16px for the white font, 28px for the yellow font). Name this file whatever you'd like
2. To match the font image you made to the character associated with it when you're typing it in a text file, go to the `config.json` file in the associated directory for your font, and add an entry for your letter. The entry is of format:
```
"file_name": "associated_character"
```
For example, if you were adding the German eszett character (`ß`), and you named the file `eszett.png`, your entry would look like:
```
"eszett": "ß"
```
3. Once you've made your desired changes, run `joiner.py`. This joins all your font characters together, writes them to `/src/font.c` and also writes several tables that need to be changed to account for changes to the font. The `y_offsets` entries in each configuration preserve the baseline of accented letters.

## Text Syntax
### Straight language ports
DK64 does several hidden things with it's text files to make text behave nicely. If you're doing a straight language port, I'd advise not messing with any tags in square brackets.
Do not mess with any words inside icon tags either in terms of converting the icon name to a different language (eg. `[icon]ButtonA[/icon]`). These icon names are just human-friendly names that feed to the converter so that it can convert those to the codes used for icons.
Each line of text in the `.txt` files is 1 entry in the text file (eg. 1 speech bubble). It is not advised to add any extra line breaks into your mod.
### More complex mods
However, if you **are** doing something other that than a straight language port, or want to know how the syntax works. All of these work in text bubbles, but not sure elsewhere:
- `[kong]` mentions the current kong's name
- `[number]` mentions a dynamic number written to by the game's code. The devs likely did this to make requirement changes quicker to make without having to mess with the text files. To change this value, change `D_global_asm_80750AC8`.
- `[wobble][/wobble]` makes any text inside it wobble
- `[pop][/pop]` makes any text do a pop effect as it appears
- `[spin][/spin]` makes any text spin when it appears
- `[icon][/icon]` displays an icon in the text. To see all values, look at the `Icons` enum in `/text_converter/converter.py`
- `[newline]` splits the text into a new line. This might have been for memory reasons on the N64?
- `[forcednewline]` splits the text into a new line. For some reason the devs needed this too?


## Things to keep an eye on
1. DK64 really doesn't like super long words because it doesn't know how to properly break things up. If possible, try to limit yourself to 13 letters or less.
2. Whilst we have a fair bit of space to do additional characters, don't go incredibly overboard. Best to check that the total amount of png files in `/font/output` is less than 50.


## Supplementary Translation Files

The normal 43 text banks remain in `text_files/` and use the original language-mod converter. Supplementary translations continue the numbering in `text_files/`: files 43 and 44. They are processed separately and do not add ROM text banks.

### Messages

Edit `text_files/[43] - Extra Text.txt` in UTF-8. Each entry has a stable identifier followed by ` = ` and its translation:

```text
WELL_DONE = MANDOU BEM!
JETPAC_EXIT = SAIR@DO@JETPAC
SPEAKER_KLUMP = KLUMP:
```

Keep the identifiers. Preserve spaces, Jetpac's `@` spaces and printf placeholders such as `%d`, `%.3s` and `%02d`. Do not add quotes around the translation. Lines starting with `#` are comments. The Arcade column labels are separate entries so each stays over its numeric column. Speaker names are editable here; character colours remain in the renderer.

Fixed game string slots have byte limits, which the converter checks against `src/fixed_text.c`. The Arcade renderer supports a limited alphabet and accent set. Changing to another language may require additional font or layout work; moving text into a TXT file does not remove those engine limits.

### Opening subtitles

Edit `text_files/[44] - Intro Subtitles.txt`. Each cue has five fields, separated by `|`:

```text
239.30|244.38|KLUMP|VOSSA EXCELÊNCIA,|JÁ CUIDAMOS DE TUDO!
254.10|255.03|KLUMP|NÃO!|
```

Fields are start time, end time, speaker, first line and second line. Times are seconds on the mod's intro clock, not timestamps in a recording. Use a decimal point. Keep the final separator for an empty second line. Supported speakers are `KROOL` and `KLUMP`; cues must be chronological and must not overlap. Keep the existing times when only revising wording.

The converter checks encoding, font coverage, line width, speaker IDs and timing order. It cannot verify that a line matches the spoken audio; play the cutscene after changing timings. Do not use `|` inside dialogue. Use uppercase text supported by the current font.

### Generate and build

To regenerate supplementary data only, run from the repository root:

```sh
python text_converter/extra_converter.py
```

This writes `include/extra_texts.h` and `include/intro_cues.h`. Both generated headers are committed so an ordinary build can use them. Do not edit those headers manually.

To regenerate both the original text banks and supplementary data:

```sh
cd text_converter
python converter.py
cd ..
```

Then build and package using the existing build dependencies:

```sh
make
RecompModTool mod.toml path/to/output
```

Running `make` alone does not regenerate TXT edits. Run the converter first. The supplementary converter uses Python 3.9 or newer and only standard-library modules. It can be run from any working directory; the original bank converter still expects `text_converter/` as its working directory.

Converter checks can be run with:

```sh
python text_converter/test_extra_converter.py
```

After editing, commit the TXT files and regenerated headers together. Test the resulting `translation_ptbr.nrm` in the game before publishing. The TXT files are build inputs, not external files loaded dynamically by an installed NRM.

## Build dependencies and packaging

Initialize the dependencies after cloning:

```sh
git submodule update --init --recursive
```

The build requires a MIPS-capable Clang, LLD, Make and RecompModTool. Font generation also requires Python and Pillow. Use MIPS2 with the hard-float ABI32; do not enable soft-float. The subtitle renderer selects F3DEX2 commands explicitly.

The build scripts package `build/mod.elf` as `bin/translation_ptbr.nrm`. Run `make` first. The Windows script also creates `bin/translation_ptbr.zip` and requires the `zip` command. Publish the NRM directly as a release asset.

## Installation

Download `translation_ptbr.nrm` from Releases, install it through the game mod menu and enable it. Disable other language mods and restart the game. DK64 Rekongpiled 1.0.2 or newer is required.
