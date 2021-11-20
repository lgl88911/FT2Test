/**
 * @file display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "display.h"

#ifndef DISPLAY_SDL_INCLUDE_PATH
#  define DISPLAY_SDL_INCLUDE_PATH <SDL2/SDL.h>
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include DISPLAY_SDL_INCLUDE_PATH


/*********************
 *      DEFINES
 *********************/
#define SDL_REFR_PERIOD     50  /*ms*/

#ifndef DISPLAY_ZOOM
#define DISPLAY_ZOOM        1
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    volatile bool sdl_refr_qry;
    uint32_t width;
    uint32_t height;
    SDL_PixelFormatEnum format;
    uint8_t pixel_byte;
    uint8_t *tft_fb;
}display_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_create(display_t * m);
static void window_update(display_t * m);
int quit_filter(void * userdata, SDL_Event * event);
static void display_sdl_clean_up(void);
static void display_sdl_init(uint32_t width, uint32_t height, display_color_t colortype);
static void display_sdl_refr_core(void);
static void display_sdl_refr_thread(void);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
display_t display;


static volatile bool sdl_inited = false;
static volatile bool sdl_quit_qry = false;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the display
 */
void display_init(uint32_t width, uint32_t height, display_color_t colortype)
{
    display_sdl_init(width, height, colortype);
}

/**
 * Flush a buffer to the marked area
 * @param drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixel to copy to the `area` part of the screen
 */
void display_write(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *buffer)
{
    int32_t c_y;

    uint32_t w = display.pixel_byte*width;
    for(c_y = y; c_y < y+height && c_y < display.height; c_y++) {
        //printf("c_y*display.width + x)*display.pixel_byte %d[%d]\n", (c_y*display.width + x)*display.pixel_byte, display.width*display.height*display.pixel_byte);
        memcpy(&display.tft_fb[(c_y*display.width + x)*display.pixel_byte], buffer, w );
        buffer += w;
    }


    display.sdl_refr_qry = true;

    //display_sdl_refr_thread();
}

void display_update(void)
{
    display_sdl_refr_thread();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */

static void display_sdl_refr_thread(void)
{
    /*Refresh handling*/
    display_sdl_refr_core();

    /*Run until quit event not arrives*/
    if(sdl_quit_qry) {
        display_sdl_clean_up();
        exit(0);
    }
}

int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if(event->type == SDL_WINDOWEVENT) {
        if(event->window.event == SDL_WINDOWEVENT_CLOSE) {
            sdl_quit_qry = true;
        }
    }
    else if(event->type == SDL_QUIT) {
        sdl_quit_qry = true;
    }

    return 1;
}

static void display_sdl_clean_up(void)
{
    SDL_DestroyTexture(display.texture);
    SDL_DestroyRenderer(display.renderer);
    SDL_DestroyWindow(display.window);

    SDL_Quit();
}

static void display_sdl_init(uint32_t width, uint32_t height, display_color_t colortype)
{
    SDL_PixelFormatEnum format;
    uint8_t pixel_byte = 0;

    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

    

    switch(colortype){
        case DISPLAY_COLOR_RGB565:
            format = SDL_PIXELFORMAT_RGB565;
            pixel_byte = 2;
        break;
        case DISPLAY_COLOR_RGB888:
            format = SDL_PIXELFORMAT_RGB888;
            pixel_byte = 3;
            break;
        default:
            format = SDL_PIXELFORMAT_ARGB8888;
            pixel_byte = 4;
            break;
    }

    display.width = width;
    display.height = height;
    display.format = format;
    display.pixel_byte = pixel_byte;
    display.tft_fb = malloc( display.width* display.height*pixel_byte);

    window_create(&display);

    sdl_inited = true;
}

static void display_sdl_refr_core(void)
{
    if(display.sdl_refr_qry != false) {
        display.sdl_refr_qry = false;
        window_update(&display);
    }

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if((&event)->type == SDL_WINDOWEVENT) {
            switch((&event)->window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(&display);
                    break;
                default:
                    break;
            }
        }
    }

}

static void window_create(display_t * m)
{
    m->window = SDL_CreateWindow("TFT Simulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              m->width * DISPLAY_ZOOM, m->height * DISPLAY_ZOOM, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    m->renderer = SDL_CreateRenderer(m->window, -1, SDL_RENDERER_SOFTWARE);
    m->texture = SDL_CreateTexture(m->renderer,
                                m->format, SDL_TEXTUREACCESS_STATIC, m->width, m->height);
    SDL_SetTextureBlendMode(m->texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    memset(m->tft_fb, 0xFF, m->width * m->height * m->pixel_byte);


    m->sdl_refr_qry = true;

}

static void window_update(display_t * m)
{

    SDL_UpdateTexture(m->texture, NULL, m->tft_fb, m->width * m->pixel_byte);

    SDL_RenderClear(m->renderer);
#if LV_COLOR_SCREEN_TRANSP
    SDL_SetRenderDrawColor(m->renderer, 0xff, 0, 0, 0xff);
    SDL_Rect r;
    r.x = 0; r.y = 0; r.w = m->width; r.w = m->hight;
    SDL_RenderDrawRect(m->renderer, &r);
#endif

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(m->renderer, m->texture, NULL, NULL);
    SDL_RenderPresent(m->renderer);
}
