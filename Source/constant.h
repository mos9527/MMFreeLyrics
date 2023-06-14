#pragma once
#include <globals.h>
// String literals
#define DEFAULT_CHARSET_NAME L"charset.txt"
#define MESSAGEBOX_TITLE L"MegaMix+ Freetype Lyrics"

#define FONTS_FOLDER_NOT_FOUND L"Fonts folder 'fonts\' not found. \n\n\
This will cause the renderer to use the built-in ImGui font (aka ProggyClean), \n\n\
which has NO SUPPORT for CJK glyphs. (in which case they get replaced by ?).\n\n\
You should 'install' the fonts by placing them (with extension .ttf/.otf) into this folder. \n\n\
Check the GitHub repo for more info."

#define LYRIC_PLACEHOLDER_MESSAGE u8"初音ミク Project DIVA FreeType Lyrics"