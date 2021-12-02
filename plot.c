#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <gcrypt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60
#define defaultTextSize 20

SDL_Rect textRect = {
    .h = defaultTextSize, .w = defaultTextSize, .x = 10, .y = 10};
SDL_Color defaultColor = {.r = 0xAA, .g = 0xAA, .b = 0xAA, .a = 0xAA};
SDL_Color white = {.r = 0xAA, .g = 0xAA, .b = 0xAA, .a = 0xFF};
SDL_Color black = {.r = 0, .g = 0, .b = 0, .a = 0xAA};
SDL_Color red = {.r = 0xFF, .g = 0, .b = 0, .a = 0xAA};
SDL_Color shade = {.r = 0, .g = 0, .b = 0, .a = 0x33};
SDL_Color transparent = {.r = 0, .g = 0, .b = 0, .a = 0};

struct internalVariables {
  double x_start;
  double y_start;
  double x_end;
  double y_end;
  int increasing;
  unsigned char animation; // value going from 0 to 100

} typedef internalVariables;
void eventHandler(SDL_Event *events, internalVariables *internalVariables) {
  while (SDL_PollEvent(events)) {
    switch (events->type) // checks wich type of event
    {
    case SDL_QUIT:
      exit(0);
    case SDL_MOUSEWHEEL:
      if (events->wheel.y < 0) {
        internalVariables->y_end += 3;
        internalVariables->y_start -= 3;
        break;
      }
      if (events->wheel.y > 0) {
        internalVariables->y_end -= 3;
        internalVariables->y_start += 3;
        break;
      }
      if (events->wheel.x < 0) {
        internalVariables->x_end += 1;
        internalVariables->x_start -= 1;
        break;
      }
      if (events->wheel.x > 0) {
        internalVariables->x_end -= 1;
        internalVariables->x_start += 1;
        break;
      }
    }
  }
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    exit(1);
  }
  if (state[SDL_SCANCODE_LEFT]) {
    internalVariables->x_start -= (double)1 / FPS;
    internalVariables->x_end -= (double)1 / FPS;
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    internalVariables->x_start += (double)1 / FPS;
    internalVariables->x_end += (double)1 / FPS;
  }
  if (state[SDL_SCANCODE_DOWN]) {
    internalVariables->y_start -= (double)1 / FPS;
    internalVariables->y_end -= (double)1 / FPS;
  }
  if (state[SDL_SCANCODE_UP]) {
    internalVariables->y_start += (double)1 / FPS;
    internalVariables->y_end += (double)1 / FPS;
  }
}
SDL_Texture *textToSurface(char *text, SDL_Renderer *screen, SDL_Rect *textRect,
                           SDL_Color fg, SDL_Color bg) {
  // render Text
  TTF_Init();
  TTF_Font *defaultFont = TTF_OpenFont("Roboto-Bold.ttf", defaultTextSize);
  char *textInfo = (char *)text;
  textRect->h = TTF_FontHeight(defaultFont);
  textRect->w = strlen(textInfo) * (defaultTextSize);
  SDL_Surface *textSurface = TTF_RenderText(defaultFont, textInfo, fg, bg);
  SDL_Texture *textTexture;
  TTF_CloseFont(defaultFont);

  return SDL_CreateTextureFromSurface(screen, textSurface);
}
void Render(SDL_Renderer *screen, internalVariables *internalVariables) {
  // clear screen
  int screenHeight;
  int screenWidth;
  SDL_GetRendererOutputSize(screen, &screenWidth, &screenHeight);

  SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(screen, 0x11, 0x11, 0x11, 200);
  SDL_RenderClear(screen);

  double plotterWidth =
      fabs(internalVariables->x_end - internalVariables->x_start);
  double plotterHeight =
      fabs(internalVariables->y_end - internalVariables->y_start);
  double virtualX; // x coordinate buffer
  double virtualY; // y coordinate buffer

  

  // plot
  SDL_SetRenderDrawColor(screen, 0xFF, 0xFF, 0xFF, 200);

  // vertical lines
  for (int i = internalVariables->x_start; i <= internalVariables->x_end; i++) {
    SDL_RenderDrawLineF(
        screen, (i - internalVariables->x_start) / plotterWidth * screenWidth,
        0, (i - internalVariables->x_start) / plotterWidth * screenWidth,
        screenHeight);
  }
  // horizontal lines
  for (int i = internalVariables->y_end; i >= internalVariables->y_start; i--) {
    SDL_RenderDrawLineF(
        screen, 0,
        (internalVariables->y_end - i) / plotterHeight * screenHeight,
        screenWidth,
        (internalVariables->y_end - i) / plotterHeight * screenHeight);
  }

  //plot functions
  SDL_SetRenderDrawColor(screen, 0, 0xFF, 0xFF, 200);
  for (int x = 0; x < screenWidth; x++) {
    virtualX=(((double)x/screenWidth)*plotterWidth+internalVariables->x_start);//x values on the plotter
    virtualY=(double)sin(virtualX);
    double projectedVirtualX=((virtualX-internalVariables->x_start)/plotterWidth)*screenWidth;//x values on screen
    double projectedVirtualY=(internalVariables->y_end-virtualY)/plotterHeight*screenHeight;
    SDL_RenderDrawLineF(screen,projectedVirtualX, internalVariables->y_end/plotterHeight*screenHeight ,projectedVirtualX, projectedVirtualY);
  }
  // render text
  char *textInfo = (char *)malloc(sizeof(char) * 100);
  SDL_Texture *textTexture;
  SDL_Rect tempTextRect = textRect;

  sprintf(textInfo, "x_start: %.3g x_end:%.3g", internalVariables->x_start,
          internalVariables->x_end);
  textTexture = textToSurface(textInfo, screen, &tempTextRect, white, shade);
  SDL_RenderCopy(screen, textTexture, NULL, &tempTextRect);
  tempTextRect.y += defaultTextSize;

  sprintf(textInfo, "y_start: %.3g y_end:%.3g", internalVariables->y_start,
          internalVariables->y_end);
  textTexture = textToSurface(textInfo, screen, &tempTextRect, white, shade);
  SDL_RenderCopy(screen, textTexture, NULL, &tempTextRect);
  tempTextRect.y += defaultTextSize;

  int mouseX,mouseY;
  double projectedMouseX,projectedMouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  projectedMouseX=(((double)mouseX/screenWidth)*plotterWidth+internalVariables->x_start);//perceived X
  projectedMouseY=-(((double)mouseY/screenHeight)*plotterHeight-internalVariables->y_end);
  sprintf(textInfo, "mouse x: %.3f mouse y:%.3f", projectedMouseX,projectedMouseY);
  textTexture = textToSurface(textInfo, screen, &tempTextRect, white, shade);
  tempTextRect.x=screenWidth-tempTextRect.w;
  tempTextRect.y=0;
  SDL_RenderCopy(screen, textTexture, NULL, &tempTextRect);
  tempTextRect.y += defaultTextSize;
  SDL_RenderPresent(screen);
}

int main(int argc, char *argv[])

{

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  SDL_DisplayMode DM;
  SDL_Event event;
  internalVariables internalVariables = {.x_start = -10,
                                         .y_start = -7,
                                         .x_end = 10,
                                         .y_end = 7,
                                         .increasing = 1,
                                         .animation = 1};
  SDL_GetDesktopDisplayMode(0, &DM);
  SDL_Window *win = SDL_CreateWindow("plotter", // creates a window
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, DM.w, DM.h, 0);
  SDL_Renderer *screen = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetWindowResizable(win, SDL_TRUE);
  while (SDL_TRUE) {
    Render(screen, &internalVariables);
    eventHandler(&event, &internalVariables);
    if (internalVariables.animation > 100 &&
        (internalVariables.increasing) > 0) {
      internalVariables.increasing = -1;
    }
    if (internalVariables.animation <= 0 &&
        (internalVariables.increasing) < 0) {
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
