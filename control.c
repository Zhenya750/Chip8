#include "control.h"

uint8_t chipSetKey(Chip_8 chip)
{
    if(chip == NULL){
        printf("Chip does not exist!\n");
        exit(EXIT_FAILURE);
    }

    SDL_Event e;

    while(SDL_PollEvent(&e)){
        switch(e.type)
        {
            case SDL_QUIT:
            {
                return END;
            }

            case SDL_KEYDOWN:
            {
                for(int i = 0; i < 16; i++){
                    if(keyboard[i] == e.key.keysym.sym){
                        getKeys(chip)[i] = 1;
                        break;
                    }
                }
                break;
            }

            case SDL_KEYUP:
            {
                for(int i = 0; i < 16; i++){
                    if(keyboard[i] == e.key.keysym.sym){
                        getKeys(chip)[i] = 0;
                        break;
                    }
                }
                break;
            }
        }
    }
    return RUN;
}
