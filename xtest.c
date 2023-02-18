#include "graph.h"
#include "grfont.h"  /* dispara^itra bientot */
#include <stdio.h>


static
void Panic( const char*  message )
{
  fprintf( stderr, "PANIC: %s\n", message );
  exit(1);
}


typedef struct grKeyName
{
  grKey       key;
  const char* name;

} grKeyName;


static
const grKeyName  key_names[] =
{
  { grKeyF1,   "F1"  },
  { grKeyF2,   "F2"  },
  { grKeyF3,   "F3"  },
  { grKeyF4,   "F4"  },
  { grKeyF5,   "F5"  },
  { grKeyF6,   "F6"  },
  { grKeyF7,   "F7"  },
  { grKeyF8,   "F8"  },
  { grKeyF9,   "F9"  },
  { grKeyF10,  "F10" },
  { grKeyF11,  "F11" },
  { grKeyF12,  "F12" },
  { grKeyEsc,  "Esc" },
  { grKeyHome, "Home" },
  { grKeyEnd,  "End"  },

  { grKeyPageUp,   "Page_Up" },
  { grKeyPageDown, "Page_Down" },
  { grKeyLeft,     "Left" },
  { grKeyRight,    "Right" },
  { grKeyUp,       "Up" },
  { grKeyDown,     "Down" },
  { grKeyBackSpace, "BackSpace" },
  { grKeyReturn,   "Return" }
};

int  main( void )
{
  grSurface*  surface;
  int         n;
  int depth=24;
  grBitmap    bit;

  bit.width = 640;
  bit.rows  = 480;
  bit.grays = 256;

  switch ( depth )
  {
    case 8:
      bit.mode = gr_pixel_mode_gray;
      break;
    case 15:
      bit.mode = gr_pixel_mode_rgb555;
      break;
    case 16:
      bit.mode = gr_pixel_mode_rgb565;
      break;
    case 24:
      bit.mode = gr_pixel_mode_rgb24;
      break;
    case 32:
      bit.mode = gr_pixel_mode_rgb32;
      break;
    default:
      bit.mode = gr_pixel_mode_none;
      break;
  }


  grInitDevices();
  surface = grNewSurface( "x11", &bit);
  if (!surface)
    Panic("Could not create window\n" );
  else
  {
    grColor      color;
    grEvent      event;
    const char*  string;
    int          x;

    //grSetSurfaceRefresh( surface, 1 );
    grSetTitle(surface,"X11 driver demonstration" );
    grFillRect(surface, 0, 0, bit.width,bit.rows, grFindColor( surface, 0xff, 0xff, 0xff, 0xff ));
    color = grFindColor( surface, 0x00, 0x00, 0xff, 0xff );
    grWriteSurfaceString( surface, 0, 0, "just an example", color );
    grRefreshSurface(surface);
    do
    {
      grListenSurface( surface, 0, &event);

      /* return if ESC was pressed */
      if ( event.key == grKeyEsc )
        return 0;

      /* otherwise, display key string */
      color.value = (color.value + 8) & 127;
      {
        int         count = sizeof(key_names)/sizeof(key_names[0]);
        grKeyName*  name  = (grKeyName*)key_names;
        grKeyName*  limit = name + count;
        const char* kname  = 0;
        char        kname_temp[16];

        while (name < limit)
        {
          if ( name->key == event.key )
          {
            kname = (const char*)name->name;
            break;
          }
          name++;
        }

        if (!kname)
        {
          sprintf( kname_temp, "char '%c'", (char)event.key );
          kname = kname_temp;
        }

        grFillRect(&bit, 30, 30, 100,60, grFindColor( &bit, 0xff, 0xff, 0xff, 0xff ));
        grWriteCellString( surface, 30, 30, kname, color );
        grRefreshSurface(surface);
      }
    } while (1);
  }

  return 0;
}

