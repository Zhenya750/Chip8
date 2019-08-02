#include "graphic.h"
#include "chip_8.h"
#include "control.h"

#define UNUSED(x) (void)x

static const char *FILENAME = "pong2";

int main(int argc, char **argv){

    UNUSED(argc);
    UNUSED(argv);

    Graphic graphic = init("Chip8 emulator");

    Chip_8 chip = initChip();
    loadProgram(chip, FILENAME);

    uint8_t state = RUN;
    while(state == RUN){

        execute(chip);

        state = chipSetKey(chip);

        if(isDrawn(chip)){
            draw(graphic, getGraphic(chip));
        }

        SDL_Delay(DISPLAY_DELAY);
    }

    destroyChip(chip);
    destroy(graphic);

    return 0;
}
