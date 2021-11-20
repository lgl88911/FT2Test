/**
 * @file display.h
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "stdint.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
typedef enum display_color{
    DISPLAY_COLOR_RGB565,
    DISPLAY_COLOR_RGB888,
}display_color_t;

void display_init(uint32_t width, uint32_t height, display_color_t colortype);
void display_write(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *buffer);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DISPLAY_H */
