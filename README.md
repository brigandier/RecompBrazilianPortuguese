# Brazilian Portuguese Translation

Brazilian Portuguese Translation of DK64 Rekongpiled

## Installation
1. Download the latest `translation_ptbr.nrm` from Releases.
2. Put the file in your DK64 Rekongpiled mods folder.
   - Example (Windows): `C:\Users\<YourUser>\AppData\Local\DK64Recompiled\mods`
3. Launch DK64 Rekongpiled and enable the mod from the mods menu.

See [PORTING.md](PORTING.md) for text editing, fonts, opening subtitles and build details.

## Build Requirements
- `clang`
- `ld.lld`
- `make`
- `RecompModTool` from [N64Recomp](https://github.com/N64Recomp/N64Recomp)

Notes:
- On macOS, Apple Clang is not enough for this target. Use an LLVM toolchain that supports MIPS and point `CC`/`LD` to it if needed.
- On Linux/macOS, ensure `zip` is installed for packaging workflows.

## Building from Source
From the repository root:

```bash
make
```

This builds `build/mod.elf`.

Then package the mod:

```bash
RecompModTool mod.toml C:/path/to/DK64Recompiled/mods
```

PowerShell example:

```powershell
.\RecompModTool.exe .\mod.toml C:\Users\<YourUser>\AppData\Local\DK64Recompiled\mods
```

The produced mod file is named `translation_ptbr.nrm`.

## Project Layout
- `src/main.c`: Text and font replacement hooks.
- `text_files/`: Brazilian Portuguese game text, supplementary messages and opening subtitles.
- `mod.toml`: Mod metadata, target game id, and packaging inputs.
- `dk64_decomp/`: Decompiled DK64 source and headers used by the build.
- `Dk64Syms/`: Symbol files used by RecompModTool.

## Credits
Brazilian Portuguese translation and project maintenance by **[brigandier](https://github.com/brigandier)**. Based on [RecompSpanish](https://github.com/theballaam96/RecompSpanish) by **Ballaam**. See [CREDITS.md](CREDITS.md) for acknowledgments.

## Feedback

Errors may still occur. Please report incorrect translations, font issues or other problems through this repository's Issues page, including a screenshot and where the issue occurred. Your feedback helps improve the mod.