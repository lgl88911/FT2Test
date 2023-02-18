#include "display.h"
#include <errno.h>
#include <locale.h>
#include <wchar.h>

int main(int argc, char **argv)
{
    display_init(640, 480);

    (void)argc; /*Unused*/
    (void)argv; /*Unused*/

    FT_Library library;
    FT_Face face;
    FT_UInt       glyph_index;
    FT_Bool       use_kerning;
    FT_UInt       previous;
    int           pen_x, pen_y, n;
    FT_GlyphSlot  slot;
    FT_Error      error;


    FT_Error err = FT_Init_FreeType(&library);
    printf("Freetype init ret = %d\n", err);

    err = FT_New_Face( library,
            "../font/LiberationSans-Regular.ttf",
            0,
            &face);
    printf("Freetype open font ret = %d %s\n", err, strerror(errno));

    err = FT_Set_Pixel_Sizes(face, 64, 0);
    printf("Freetype set Pixel ret = %d\n", err);

    use_kerning = FT_HAS_KERNING( face );
    printf("Freetype use_kerning ret = %d\n", use_kerning);

    pen_x = 50;
    pen_y = 50;
    slot = face->glyph;

    //描边器
    FT_Fixed  radius;
    FT_Stroker  stroker;
    FT_Stroker_New( library, &stroker );
    radius = face->size->metrics.y_ppem * 64 * 0.05;        //0~0.05 , 0.005 step
    FT_Stroker_Set( stroker, radius,
            FT_STROKER_LINECAP_ROUND,
            FT_STROKER_LINEJOIN_ROUND,
            0 );

    //斜体
    FT_Matrix  shear;
    shear.xx = 1 << 16;
    shear.xy = (FT_Fixed)( 0.2 * ( 1 << 16 ) ); //-1.0~1.0, 0.02 step
    shear.yx = 0;
    shear.yy = 1 << 16;

    //加粗器
    FT_Pos     xstr, ystr;
    xstr = (FT_Pos)( face->size->metrics.y_ppem * 64 * 0.05 ); //-1.0~1.0, 0.005 step
    ystr = (FT_Pos)( face->size->metrics.y_ppem * 64 * 0.05 );

    wchar_t  *text = L"use_kerning";
    for ( n = 0; n < wcslen(text); n++ )
    {
        /* convert character code to glyph index */
        glyph_index = FT_Get_Char_Index( face, text[n] );

        /* retrieve kerning distance and move pen position */
        if ( use_kerning && previous && glyph_index )
        {
        FT_Vector  delta;


        FT_Get_Kerning( face, previous, glyph_index,
                        FT_KERNING_UNFITTED, &delta );

        pen_x += delta.x >> 6;
        }

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_FORCE_AUTOHINT | FT_LOAD_NO_BITMAP);
        if ( error )
        continue;  /* ignore errors */
        printf("slot format %d\n", slot->format);


        if(slot->format == FT_GLYPH_FORMAT_OUTLINE){
            FT_Glyph glyph;


            //斜体处理
            FT_Outline_Transform( &slot->outline, &shear );

            //粗体处理
            FT_Outline_EmboldenXY( &slot->outline, xstr, ystr );

            //获取字形
            FT_Get_Glyph(slot, &glyph);

            //字形描边
            FT_Glyph_Stroke( &glyph, stroker, 1 );

            if ( slot->advance.x )
                slot->advance.x += xstr;
/*
            if ( slot->advance.y )
                slot->advance.y += ystr;

            slot->metrics.width        += xstr;
            slot->metrics.height       += ystr;
            slot->metrics.horiAdvance  += xstr;
            slot->metrics.vertAdvance  += ystr;
*/
            //显示字形
            display_glyph(pen_x, pen_y, library, glyph);
        }else{
            /* now draw to our target surface */
            printf("draw %d: pixel mode %d num_grays %d palette_mode %d\n", glyph_index, slot->bitmap.pixel_mode, slot->bitmap.num_grays, slot->bitmap.palette_mode);
            display_bitmap(  pen_x + slot->bitmap_left,
                            pen_y - slot->bitmap_top,
                            slot->bitmap.width,
                            slot->bitmap.rows,
                            slot->bitmap.pitch,
                            slot->bitmap.buffer);
        }


        /* increment pen position */
        pen_x += slot->advance.x >> 6;

        /* record current glyph index */
        previous = glyph_index;
    }

    display_refresh();

    display_exit();
}