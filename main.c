#include "display.h"
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  FT_Library library;
  FT_Face face;
  //setlocale(LC_ALL,"zh_CN.UTF-8");

  uint16_t patten[100*100];
  for(int i=0;i<100*100; i++){
      patten[i] = 0xF800;
  }

  display_init(800, 600, DISPLAY_COLOR_RGB565);

  display_write(50, 50, 20, 20, patten);

  FT_Error err = FT_Init_FreeType(&library);
  printf("Freetype init ret = %d\n", err);

  err = FT_New_Face( library,
        "../font/SourceHanSansCN-Normal.otf",
        0,
        &face);
  printf("Freetype open font ret = %d %s\n", err, strerror(errno));

  //FILE *fp = fopen("../bin/arial.ttf", "r");
  //printf("open font ret = %p:%s\n", fp, strerror(errno));

  err = FT_Set_Pixel_Sizes(face, 64, 0);
  printf("Freetype set Pixel ret = %d\n", err);

  FT_ULong c = L'李';
  err = FT_Load_Char(face, c, FT_LOAD_RENDER);
  printf("Freetype load char ret = %d c = %x\n", err, c);


  FT_GlyphSlot slot = face->glyph;

  uint32_t width = slot->bitmap.width;
  uint32_t height = slot->bitmap.rows;
  uint8_t *buffer = slot->bitmap.buffer;
  
  uint8_t pixel_format = slot->bitmap.pixel_mode;
  printf("Font %d*%d buf %p[%d] pixel %d\n", width, height, buffer, strlen(slot->bitmap.buffer), pixel_format);

  for(int j = 0; j<height; j++){
    for(int i =0; i<width; i++){
      printf("%02x", buffer[i+width*j]);
    }
    printf("\n");
  }

  uint32_t size = width*height;
  uint16_t *buffer16 = malloc(size*2);
  for(int j=0; j<size; j++){
    buffer16[j] = buffer[j]?0x0000:0xFFFF;
  }

  display_write(10, 10, width, height, buffer16);

  free(buffer16);

  wchar_t  *s = L"测试FreeType 2横向";
  //err = FT_Set_Pixel_Sizes(face, 64, 0);
  int32_t base_x=10, base_y=200;
  printf("wcslen %d\n", wcslen(s));
  for(int i=0; i<wcslen(s); i++){
    err = FT_Load_Char(face, s[i], FT_LOAD_RENDER);

    FT_GlyphSlot slot = face->glyph;

    uint32_t width = slot->bitmap.width;
    uint32_t height = slot->bitmap.rows;
    uint8_t *buffer = slot->bitmap.buffer;

    for(int j = 0; j<height; j++){
      for(int i =0; i<width; i++){
        printf("%02x", buffer[i+width*j]);
      }
      printf("\n");
    }

    uint32_t size = width*height;
    uint16_t *buffer16 = malloc(size*2);
    #define COLORING_MIX(_grey, _f, _b) (((0xFF-_grey)*_b + _grey*_f)/0xFF)
    #define COLORING_RGB565(_grey, _FR, _FG, _FB, _BR, _BG, _BB)  ((COLORING_MIX(_grey,_FR,_BR)>>3)<<11) | (((COLORING_MIX(_grey,_FG,_BG)>>2)&0x3F)<<5) | ((COLORING_MIX(_grey,_FB,_BB)>>3)&0x1F)
    for(int j=0; j<size; j++){
      #if 0
      uint16_t R=buffer[j]>>3;    //8->5
      uint16_t G=buffer[j]>>2;    //8->6
      uint16_t B=buffer[j]>>3;    //8->5
      uint16_t RGB565 = (R<<11) | (G<<5) | B;
      #else
      uint16_t grey = buffer[j];
      uint16_t RGB565 = COLORING_RGB565(grey, 0, 0, 255, 255, 255, 255);
      #endif
      buffer16[j] = RGB565;
    }

    printf("Font %d*%d buf %p x %d y %d\n", width, height, buffer, base_x+slot->bitmap_left, base_y-slot->bitmap_top);

    display_write(base_x+slot->bitmap_left, base_y-slot->bitmap_top, width, height, buffer16);
    base_x += (slot->advance.x >> 6);

    free(buffer16);
  }

  while (1) {
    usleep(5 * 1000);
    display_update();
  }

  return 0;
}