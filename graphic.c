#include "graphic.h"

struct _graphic
{
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *buffer;
};

struct _graphic* init(const char *title)
{
    struct _graphic *graphic = (struct _graphic*) malloc(sizeof(struct _graphic));

    graphic->window = NULL;
    graphic->screen = NULL;
    graphic->buffer = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL can not be initialized! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    else{
        graphic->window = SDL_CreateWindow(title,
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           64 * SCREEN_SCALE,
                                           32 * SCREEN_SCALE,
                                           SDL_WINDOW_SHOWN);

        if(graphic->window == NULL){
            printf("Window can not be created :( SDL_Error: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }

    graphic->screen = SDL_GetWindowSurface(graphic->window);
    graphic->buffer = SDL_CreateRGBSurface(0, 64 * SCREEN_SCALE, 32 * SCREEN_SCALE, 32, 0, 0, 0, 0);

    if(graphic->screen == NULL || graphic->buffer == NULL){
        printf("Screen or buffer can not be created :( SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return graphic;
}

void destroy(struct _graphic *graphic)
{
    if(graphic != NULL){
        SDL_FreeSurface(graphic->buffer);
        SDL_FreeSurface(graphic->screen);
        SDL_DestroyWindow(graphic->window);

        SDL_Quit();
    }
}

void draw(struct _graphic *graphic, uint8_t *binaryArray)
{
    if(graphic == NULL){
        printf("A struct Graphic does not exist!\n");
        exit(EXIT_FAILURE);
    }
    if(binaryArray == NULL){
        printf("Binary array does not exist!\n");
        exit(EXIT_FAILURE);
    }

    SDL_Rect pen = { 0, 0, SCREEN_SCALE, SCREEN_SCALE };

    for(int ylen = 0; ylen < 32; ylen++){
        for(int xlen = 0; xlen < 64; xlen++){

            if(binaryArray[xlen + (ylen * 64)] == 0){
                SDL_FillRect(graphic->buffer, &pen, SDL_MapRGB(graphic->screen->format, 0, 0, 0));
            }
            else{
                SDL_FillRect(graphic->buffer, &pen, SDL_MapRGB(graphic->screen->format, 0xFF, 0xFF, 0xFF));
            }
            pen.x += SCREEN_SCALE;

        }
        pen.x = 0;
        pen.y += SCREEN_SCALE;
    }

    SDL_BlitSurface(graphic->buffer, NULL, graphic->screen, NULL);
    SDL_UpdateWindowSurface(graphic->window);
}

