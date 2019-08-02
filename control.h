#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL.h>
#include "chip_8.h"

enum WORK_STATE { RUN, END };

/**
 * @brief Array of keyboard buttons for a Chip
 */
static const SDL_Keycode keyboard[16] =
{
    SDLK_KP_0, SDLK_KP_1,
    SDLK_KP_2, SDLK_KP_3,
    SDLK_KP_4, SDLK_KP_5,
    SDLK_KP_6, SDLK_KP_7,
    SDLK_KP_8, SDLK_KP_9,
    SDLK_a,    SDLK_b,
    SDLK_c,    SDLK_d,
    SDLK_e,    SDLK_f
};

/**
 * @brief
 * Start key event cycle.
 * Set elements in key array if corresponding buttons are pressed
 * @param chip
 * @return element of the WORK_STATE
 */
uint8_t chipSetKey(Chip_8 chip);

#endif // KEYBOARD_H
