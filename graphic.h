#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief
 * width/height of the screen = 64/32.
 * For correct display SCREEN_SCALE should be in 5..30
 */
static const uint8_t SCREEN_SCALE = 10;

/**
 * @brief Time to delay the next instruction execution
 */
static const uint8_t DISPLAY_DELAY = 2;

/**
 * @brief Graphic structure for display
 */
typedef struct _graphic* Graphic;

/**
 * @brief creates a struct graphic with window
 * @param title
 * @return a pointer to the struct graphic
 */
Graphic init(const char *title);

/**
 * @brief frees memory for the struct graphic
 * @param graphic
 */
void destroy(Graphic graphic);  //print some msg

/**
 * @brief draws corresponding binaryArray. Each element is a square
 * @param Graphic object
 * @param binaryArray
 */
void draw(Graphic graphic, uint8_t *binaryArray);

#endif // GRAPHIC_H
