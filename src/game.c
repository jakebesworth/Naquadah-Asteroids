/* Language Includes */
#ifndef __CSTD__
#define __CSTD__
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#ifndef __CSYSTEM__
#define __CSYSTEM__
#include <unistd.h>
#include <time.h>
#ifndef F_OK
#define F_OK 0
#endif
#endif

/* SDL Library */
#ifndef __SDL__
#define __SDL__
#include <SDL.h>
#endif

/* Local Includes */
#ifndef __TYPES__
#define __TYPES__
#include "types.h"
#endif

#ifndef __WRAPPERS__
#define __WRAPPERS__
#include "wrappers.h"
#endif

#ifndef __GAME__
#define __GAME__
#include "game.h"
#endif

#ifndef __GLOBAL__
#define __GLOBAL__
#include "global.h"
#endif

void setup()
{
    /* Specifically to avoid return warnings and unused variable warnings */
    FILE * temp = freopen(ERROR_FILE, "a", stderr);
    (void)temp;

    /* Seed srand */
    srand(time(NULL));
    
    /* Initialize Keystates */
    keystates = SDL_GetKeyboardState(NULL);

    /* Constant Logic / initialize */
    SCREEN_WIDTH = 720;
    SCREEN_HEIGHT = 405;
    SCREEN_TOP = (SCREEN_HEIGHT * 0.15);
    SCREEN_BOTTOM = 40;
    SCREEN_LEFT = 5;
    SCREEN_RIGHT = 40;
    FRAMES_PER_SECOND = 60; /* Up to 144 */
    GAME_TICK_RATIO = (60.0 / FRAMES_PER_SECOND);

    SHIP_SPEED = (7.5 * GAME_TICK_RATIO);
    ASTEROID_SPEED = (1.5 * GAME_TICK_RATIO);
}

Object * updateAsteroids(Object * asteroids, SDL_Surface * image)
{
    Object * asteroidsRoot;
    Object * asteroid;
    Object * previousAsteroid;
    uint8_t random;
    uint8_t root = 1;

    if((rand() % 20) == 1)
    {
        random = rand() % 6;

        if(random >= 3)
        {
            asteroid = createObject(image, 0, 1, ASTEROID_SMALL, 0, 32, 32, 32);
        }
        else if(random >= 1)
        {
            asteroid = createObject(image, 0, 1, ASTEROID_MEDIUM, 32, 32, 64, 64);
        }
        else
        {
            asteroid = createObject(image, 0, 1, ASTEROID_LARGE, 96, 32, 96, 96);
        }

        asteroid->x = (int) ((rand() % (SCREEN_WIDTH)) - 32);
        asteroid->y = -96;

        asteroid->next = asteroids;
        asteroids = asteroid;
    }

    asteroidsRoot = asteroids;

    while(asteroids != NULL)
    {
        if(asteroids->y > (SCREEN_HEIGHT + asteroids->clip.h))
        {
            asteroid = asteroids;
            asteroids = asteroids->next;

            if(root)
            {
                asteroidsRoot = asteroids;
            }
            else
            {
                previousAsteroid->next = asteroids;
            }

            asteroid->next = NULL;
            freeObjects(asteroid);
            continue;
        }

        previousAsteroid = asteroids;

        moveObject(asteroids, 0, ASTEROID_SPEED);
        asteroids = asteroids->next;

        if(root)
        {
            root = 0;
        }
    }

    return asteroidsRoot;
}

void updateUserActions(Object * ship)
{
    int8_t shipX = 0;
    int8_t shipY = 0;

    /* User Keyboard  */
    if(keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A])
    {
        shipX--;
    }

    if(keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D])
    {
        shipX++;
    }

    if(keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W])
    {
        shipY--;
    }

    if(keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S])
    {
        shipY++;
    }

    /* Updating Ship Animation */
    if(shipX == 0)
    {
        ship->subImage = 0;
    }    
    else if(shipX == -1)
    {
        ship->subImage = 1;
    }
    else if(shipX == 1)
    {
        ship->subImage = 2;
    }

    /* Setting Ship Boundaries */
    if((ship->x + (shipX * SHIP_SPEED)) < (0 + SCREEN_LEFT))
    {
        shipX = 0;
    }

    if((ship->x + (shipX * SHIP_SPEED)) > (SCREEN_WIDTH - SCREEN_RIGHT))
    {
        shipX = 0;
    }

    if((ship->y + (shipY * SHIP_SPEED)) < (0 + SCREEN_TOP))
    {
        shipY = 0;
    }

    if((ship->y + (shipY * SHIP_SPEED)) > (SCREEN_HEIGHT - SCREEN_BOTTOM))
    {
        shipY = 0;
    }

    moveObject(ship, shipX * SHIP_SPEED, shipY * SHIP_SPEED);
}

void updateObjectAnimation(Object * obj)
{
    obj->subImage = ((obj->subImage + 1) >= obj->subImageNumber) ? 0 : (obj->subImage + 1);
}

void positionObject(Object * obj, int x, int y)
{
    SDL_Rect clip = obj->clip;

    obj->x = x;
    obj->y = y;
    clip.x += clip.w * obj->subImage;
    applySurface(obj->x, obj->y, obj->image, &clip);
}

void moveObject(Object * obj, int x, int y)
{
    SDL_Rect clip = obj->clip;

    obj->x += x;
    obj->y += y;
    clip.x += clip.w * obj->subImage;
    applySurface(obj->x, obj->y, obj->image, &clip);
}

Object * createObject(SDL_Surface * image, int subImage, int subImageNumber, objectType type, int x, int y, int w, int h)
{
    Object * obj = NULL;

    obj = malloc(sizeof(Object));

    if(obj == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Fatal Error: Memory allocation error\n", getDate(), __FILE__, __LINE__);
        exit(0);
    }

    obj->image = image;
    obj->subImage = subImage;
    obj->subImageNumber = 3;
    obj->x = obj->y = 0;
    obj->type = type;

    obj->clip.x = x;
    obj->clip.y = y;
    obj->clip.w = w;
    obj->clip.h = h;

    obj->next = NULL;

    return obj;
}

int countObjects(Object * obj)
{
    int count = 0;

    while(obj != NULL)
    {
        count++;
        obj = obj->next;
    }

    return count;
}


void freeObjects(Object * obj)
{
    Object * temp = NULL;

    while(obj != NULL)
    {
        temp = obj;
        obj = obj->next;
        free(temp);
    }
}
