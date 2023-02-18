#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <ft2build.h>
#include <freetype/ftstroke.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

void display_init(int width, int hight);
void display_deinit(void);
void display_refresh(void);
void display_write(int x, int y, int width, int hight, int pitch, char *buffer);

#endif