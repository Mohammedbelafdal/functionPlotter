#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <gcrypt.h>
#include <math.h>

#define FPS 60

struct internalVariables
{
    double x_start;
    double y_start;
    double x_end;
    double y_end;
    int increasing;
    unsigned char animation; //value going from 0 to 100

} typedef internalVariables;
void eventHandler(SDL_Event *events, internalVariables *internalVariables)
{
    while (SDL_PollEvent(events))
    {
        switch (events->type) //checks wich type of event
        {
        case SDL_QUIT:
            exit(0);
        case SDL_MOUSEWHEEL:
        if(events->wheel.y<0)
        {
            internalVariables->y_end+=1;
            internalVariables->y_start-=1;
            break;
        }
        if(events->wheel.y>0)
        {
            internalVariables->y_end-=1;
            internalVariables->y_start+=1;
            break;
        }
        if(events->wheel.x<0)
        {
            internalVariables->x_end+=1;
            internalVariables->x_start-=1;
            break;
        }
        if(events->wheel.x>0)
        {
            internalVariables->x_end-=1;
            internalVariables->x_start+=1;
            break;
        }
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        exit(1);
    }
    if (state[SDL_SCANCODE_LEFT])
    {
        internalVariables->x_start-=(double)1/FPS;
        internalVariables->x_end-=(double)1/FPS;
    }
    if (state[SDL_SCANCODE_RIGHT])
    {
        internalVariables->x_start+=(double)1/FPS;
        internalVariables->x_end+=(double)1/FPS;
    }
    if (state[SDL_SCANCODE_DOWN])
    {
        internalVariables->y_start-=(double)1/FPS;
        internalVariables->y_end-=(double)1/FPS;
    }
    if (state[SDL_SCANCODE_UP])
    {
        internalVariables->y_start+=(double)1/FPS;
        internalVariables->y_end+=(double)1/FPS;
    }
}
double function(double x, double a)
{
    return sin(a*x)/x;
}
double function2(double x, double a)
{
    return fabs(x)<a;
}
void Render(SDL_Renderer *screen, internalVariables *internalVariables)
{
    int height;
    int width;
    SDL_GetRendererOutputSize(screen, &width, &height);
    SDL_RenderClear(screen);
    SDL_SetRenderDrawColor(screen, 0, 255, 255, 255);
    double TrueX;
    double Truey;
    //double y;
    double x_boundary = fabs(internalVariables->x_end - internalVariables->x_start);
    double y_boundary = fabs(internalVariables->y_start - internalVariables->y_end);

    double virtualZeroX = -(internalVariables->x_start) / x_boundary * width; //virtual coordinates of the zero
    double virtualZeroY = (internalVariables->y_end) / y_boundary * height;
    //drawing axis
    SDL_SetRenderDrawColor(screen, 255, 255, 255, 255);
    SDL_RenderDrawLine(screen, virtualZeroX, 0, virtualZeroX, height);
    SDL_RenderDrawLine(screen, 0, virtualZeroY, width, virtualZeroY);

    SDL_SetRenderDrawColor(screen, 255, 0, 0, 255);
    for (int x = 1; x <= 20; x++) //drawing thin vertical axis
    {
        SDL_RenderDrawLine(screen, virtualZeroX + x * width / x_boundary / 2, 0, virtualZeroX + x * width / x_boundary / 2, height);
        SDL_RenderDrawLine(screen, virtualZeroX - x * width / x_boundary / 2, 0, virtualZeroX - x * width / x_boundary / 2, height);
    }
    for (int x = 1; x <= 15; x++) //drawing thin horizontal axis
    {
        SDL_RenderDrawLine(screen, 0, virtualZeroY - x * width / 2 / y_boundary, width, virtualZeroY - x * width / 2 / y_boundary);
        SDL_RenderDrawLine(screen, 0, virtualZeroY + x * width / 2 / y_boundary, width, virtualZeroY + x * width / 2 / y_boundary);
    }
    SDL_SetRenderDrawColor(screen, 0, 0xAA, 0xAA, 255);
    for (int x = 0; x < width; x++)
    {
        TrueX = (x - virtualZeroX) * x_boundary / width;
        Truey = function(TrueX, (double)internalVariables->animation/15) * 2 / y_boundary;
        SDL_SetRenderDrawColor(screen, 0, 0xAA, 0xAA, 100);
        SDL_RenderDrawLine(screen, x,virtualZeroY,x, -Truey * height / 2 + virtualZeroY);
        SDL_RenderDrawPointF(screen, x, -Truey * height / 2 + virtualZeroY);
        TrueX = (x - virtualZeroX) * x_boundary / width;
        Truey = function2(TrueX,(double)internalVariables->animation/15) * 2 / y_boundary;
        SDL_SetRenderDrawColor(screen, 0, 0xAA, 0, 100);
        SDL_RenderDrawLine(screen, x,virtualZeroY,x, -Truey * height / 2 + virtualZeroY);
        SDL_RenderDrawPointF(screen, x, -Truey * height / 2 + virtualZeroY);
    }
    SDL_SetRenderDrawColor(screen, 0, 0xAA, 0, 255);

    // for (int x = 0; x < width; x++)
    // {
    //     TrueX = (x - virtualZeroX) * x_boundary / width;
    //     Truey = function2(TrueX,(double)internalVariables->animation/10) * 2 / y_boundary;
    //     SDL_RenderDrawLine(screen, x,virtualZeroY,x, -Truey * height / 2 + virtualZeroY);
    //     SDL_RenderDrawPointF(screen, x, -Truey * height / 2 + virtualZeroY);
    // }

    //finally render
    SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
    SDL_RenderPresent(screen);
}
int main(int argc, char *argv[])

{

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_DisplayMode DM;
    SDL_Event event;
    internalVariables internalVariables = {.x_start = -10, .x_end = 10, .y_start = -7, .y_end = 7, .increasing = 1, .animation = 1};
    SDL_GetDesktopDisplayMode(0, &DM);
    SDL_Window *win = SDL_CreateWindow("plotter", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       DM.w, DM.h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetWindowResizable(win, SDL_TRUE);
    while (SDL_TRUE)
    {
        Render(renderer, &internalVariables);
        eventHandler(&event, &internalVariables);
        if (internalVariables.animation > 100 && (internalVariables.increasing) > 0)
        {
            internalVariables.increasing = -1;
        }
        if (internalVariables.animation <= 0 && (internalVariables.increasing) < 0)
        {
            internalVariables.increasing = 1;
        }
        internalVariables.animation += internalVariables.increasing;
        SDL_Delay(1000 / FPS);
    }

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}
