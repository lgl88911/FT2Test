#include "graph.h"
#include "grfont.h"  /* dispara^itra bientot */
#include <stdio.h>
#include "display.h"

static grSurface*  surface;

static
void panic( const char*  message )
{
  fprintf( stderr, "PANIC: %s\n", message );
  exit(1);
}

void display_init(int width, int hight)
{
    grBitmap    bit;

    bit.width = width;
    bit.rows  = hight;
    bit.grays = 256;
    bit.mode = gr_pixel_mode_rgb24;

    grInitDevices();

    surface = grNewSurface( "x11", &bit);
    if (!surface){
        panic("Could not create window\n" );
    }

    grSetTitle(surface,"X11 driver demonstration" );
    grFillRect(surface, 0, 0, bit.width, bit.rows, grFindColor( surface, 0xff, 0xff, 0xff, 0xff ));
    grRefreshSurface(surface);
}

void display_deinit(void)
{
    grDoneSurface(surface);
}

void display_refresh(void)
{
    grRefreshSurface(surface);
}

void display_bitmap(int x, int y, int width, int hight, int pitch, char *buffer)
{
    grBitmap    bit;
    bit.buffer = buffer;
    bit.width = width;
    bit.rows = hight;
    bit.pitch = pitch;
    bit.mode = gr_pixel_mode_gray;
    bit.grays = 256;
    printf("write %d %d %d %d pitch %d buffer %p\n", x, y, width, hight, pitch, buffer);
    grBlitGlyphToSurface(surface, &bit, x, y, grFindColor( surface, 0x00, 0x00, 0xff, 0xff ));
}

void display_glyph(int x, int y, FT_Library library, FT_Glyph glyf)
{
  FT_BitmapGlyph  bitmap;
  FT_Bitmap*      source;
  FT_Bitmap       target_ft;
  grBitmap bit;
  grBitmap  *target = &bit;
  FT_Error error;
  FT_Glyph aglyf;

  if ( glyf->format == FT_GLYPH_FORMAT_OUTLINE ||
        glyf->format == FT_GLYPH_FORMAT_SVG     )
  {
    FT_Render_Mode  render_mode = FT_RENDER_MODE_LCD;

    /* render the glyph to a bitmap, don't destroy original */
    error = FT_Glyph_To_Bitmap( &glyf, render_mode, NULL, 0 );
    if ( error )
      return error;

    aglyf = glyf;
  }

  if ( glyf->format != FT_GLYPH_FORMAT_BITMAP )
    panic( "invalid glyph format returned!" );

  bitmap = (FT_BitmapGlyph)glyf;
  source = &bitmap->bitmap;

  target->rows   = (int)source->rows;
  target->width  = (int)source->width;
  target->pitch  = source->pitch;
  target->buffer = source->buffer;  /* source glyf still owns it */
  target->grays  = source->num_grays;

  printf("source->pixel_mode %d source->num_grays %d\n", source->pixel_mode, source->num_grays);
  switch ( source->pixel_mode )
  {
  case FT_PIXEL_MODE_MONO:
    target->mode = gr_pixel_mode_mono;
    break;

  case FT_PIXEL_MODE_GRAY:
    target->mode  = gr_pixel_mode_gray;
    target->grays = source->num_grays;
    break;

  case FT_PIXEL_MODE_GRAY2:
  case FT_PIXEL_MODE_GRAY4:
    (void)FT_Bitmap_Convert( library, source, &target_ft, 1 );
    target->pitch  = target_ft.pitch;
    target->buffer = target_ft.buffer;
    target->mode   = gr_pixel_mode_gray;
    target->grays  = target_ft.num_grays;
    break;

  case FT_PIXEL_MODE_LCD:
    target->mode  = gr_pixel_mode_lcd;
    target->grays = source->num_grays;
    break;

  case FT_PIXEL_MODE_LCD_V:
    target->mode  = gr_pixel_mode_lcdv;
    target->grays = source->num_grays;
    break;

  case FT_PIXEL_MODE_BGRA:
    target->mode  = gr_pixel_mode_bgra;
    target->grays = source->num_grays;
    break;

  default:
    return FT_Err_Invalid_Glyph_Format;
  }


  // *x_advance = ( glyf->advance.x + 0x8000 ) >> 16;
  // *y_advance = ( glyf->advance.y + 0x8000 ) >> 16;

  if ( error )
  {
    FT_Done_Glyph( glyf );

    return error;
  }
  grBlitGlyphToSurface(surface, target, x+bitmap->left, y-bitmap->top, grFindColor( surface, 0xff, 0x00, 0x00, 0xff ));

  if ( glyf )
      FT_Done_Glyph( glyf );
  return error;
}

void display_exit(void)
{
    grEvent      event;

    do
    {
      grListenSurface( surface, 0, &event);

      /* return if ESC was pressed */
      if ( event.key == grKeyEsc ) {
        exit(0);
      }
    }while(1);
}