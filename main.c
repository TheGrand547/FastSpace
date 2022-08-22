#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "array.h"
#include "button.h"
#include "constants.h"
#include "debug_display.h"
#include "draw.h"
#include "font.h"
#include "misc.h"
#include "names.h"
#include "player.h"
#include "setup.h"
#include "ship.h"
#include "ship_types.h"
#include "super_header.h"
// TODO: put these in their proper places


// TODO: Find an appropriate place for these
#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

#define SET_FLAG 1
#define CLEAR_FLAG 0

// TODO: it's clera why this is bad
size_t check_if_pointer_exists_in_collision(void *data);
void temp_collision_clean();
void temp_collision_thing(void *ship);


SDL_Renderer *GameRenderer;
SDL_Window *GameWindow;
Field GameField = {WIDTH, HEIGHT, RECT_X, RECT_Y, SPACING, SPACING, SPACING};

// TODO: THIS IS HORRIFIC PLEASE FIX IT WHEN YOU GET TO MENUS AND STUFF
static struct
{
    const Uint16 turnDelay;
} userSettings = {250}; // 250 is what it was before, that felt a tad slow

static Array *badBullets;
static Array *goodBullets;
static Array *miscShips;
static void **collisionHolder;

// TODO: Have array of all the different arrays that hold ships so it can be easily tracked

int main(int argc, char **argv)
{
    int loop = 1;
    InitDebugDisplay(argc, argv);

    if (InitializeLibraries())
    {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return -1;
    }
    SDL_version version;
    SDL_VERSION(&version);
    printf("SDL VERSION: %i %i %i\n", version.major, version.minor, version.patch);

    /*
    SDL_RendererInfo info;
    *
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
    {
        SDL_GetRenderDriverInfo(i, &info);
        printf("Renderer Name: %s\n", info.name);
        printf("%s %i\n", STR(SDL_RENDERER_ACCELERATED), (info.flags & SDL_RENDERER_ACCELERATED) > 0);
        printf("%s %i\n", STR(SDL_RENDERER_SOFTWARE), (info.flags & SDL_RENDERER_SOFTWARE) > 0);
        printf("%s %i\n", STR(SDL_RENDERER_PRESENTVSYNC), (info.flags & SDL_RENDERER_ACCELERATED) > 0);
        printf("%s %i\n", STR(SDL_RENDERER_TARGETTEXTURE), (info.flags & SDL_RENDERER_ACCELERATED) > 0);
    }*
    SDL_GetRendererInfo(GameRenderer, &info);
    printf("Renderer Name: %s\n", info.name);
    printf("Render to Texture: %i %i\n", info.flags & SDL_RENDERER_TARGETTEXTURE, SDL_RENDERER_TARGETTEXTURE);*/


    Array* ships = ArrayNew();
    Ship *player = CreatePlayer(0, 0, RIGHT);

    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);

    collisionHolder = calloc(NumTiles(), sizeof(void*));
    badBullets = ArrayNew();
    goodBullets = ArrayNew();
    miscShips = ArrayNew();
    if (!collisionHolder || !badBullets || !goodBullets || !miscShips)
    {
        printf("Failure allocating arrays\n");
        return -1;
    }

    ArrayAppend(ships, CreateCircle(2, 4, RIGHT));
    ArrayAppend(ships, CreateCircle(8, 4, LEFT));
    ArrayAppend(ships, CreateExplosion(3, 3, RIGHT));

    ColorShip(player, SDL_MapRGB(DisplayPixelFormat, 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Action selection = NO_ACTION;
    uint32_t turnTimer = 0;

    // TODO: Move this externally
    struct
    {
        uint8_t switchTurn : 1; // 5 unused
        uint8_t windowSize : 1;
        uint8_t bufferState : 1;
        uint8_t doCollision : 1;
        uint8_t goob : 1;
    } flags;
    flags.windowSize = SET_FLAG;
    flags.switchTurn = CLEAR_FLAG;
    flags.bufferState = CLEAR_FLAG;
    flags.doCollision = CLEAR_FLAG;

    // Why the hell did I do things like this
    EnableDebugDisplay(SHOW_FPS, TOP_RIGHT, NULL);
    EnableDebugDisplay(SHOW_TURN, TOP_RIGHT, &turn);

    LoadShipImages(); // HACKY

    void *selected_ship = NULL; // Currently selected ship
    SDL_Texture *selected_texture = NULL;
    SDL_Rect selected_rect;

    // Throwaway variables
    SDL_Rect rect;
    Ship *s = NULL;
    void **dp = NULL; // Dummy double pointer(no immature jokes)
    UNUSED(dp);

    while (loop)
    {
        const uint32_t frameStartTick = SDL_GetTicks();

        SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(GameRenderer);

        unsigned int turnAdvance = (turn == PLAYER) && (selection != NO_ACTION);

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
                break;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) // All mouse events
            {
                // This is a hack
                if (turnAdvance && ButtonCheck(button, &e))
                    flags.switchTurn = SET_FLAG;
                else if (e.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_Point mouse_pos = (SDL_Point) {e.button.x, e.button.y};
                    selected_ship = NULL;
                    for (size_t i = 0; i < ArrayLength(ships); i++)
                    {
                        s = (Ship*) ArrayElement(ships, i);
                        if (s) {
                            rect = GetTile(s->x, s->y);
                            if (SDL_PointInRect(&mouse_pos, &rect))
                            {
                                selected_ship = s;
                                DESTROY_SDL_TEXTURE(selected_texture);
                            }
                        }
                    }
                }
            }
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.scancode)
                {
                    case SDL_SCANCODE_Q:
                        loop = 0;
                        break;
                    case SDL_SCANCODE_G:
                    {
                        flags.goob = ~flags.goob;
                        Ship *bullet = CreateBullet(1, 6, RIGHT);
                        NULL_CONTINUE(bullet);
                        ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
                        ArrayAppend(goodBullets, bullet);
                        break;
                    }
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        selection = TURN_LEFT;
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        selection = TURN_RIGHT;
                        break;
                    case SDL_SCANCODE_SPACE:
                        selection = SHOOT;
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_RETURN2:
                        flags.switchTurn = turnAdvance;
                        break;
                    case SDL_SCANCODE_O:
                        GameField.spacing += 2;
                        flags.windowSize = SET_FLAG;
                        break;
                    case SDL_SCANCODE_L:
                        GameField.spacing -= 2;
                        flags.windowSize = SET_FLAG;
                        break;
                    default:
                        break;
                }
            }
        }

        // AI turn handling
        if (turn == AI)
        {
            turn = PLAYER_BUFFER;
            turnTimer = frameStartTick;

            ArrayIterate(ships, ActivateShip);      // Activate enemies
            ArrayIterate(badBullets, ActivateShip); // Activate enemy bullets

            flags.doCollision = SET_FLAG;
        }

        // Flag handling
        {
            if (flags.windowSize)
            {
                if (!GameField.spacing || !(GameField.spacing & 1))
                    GameField.spacing += 1;
                SDL_SetWindowSize(GameWindow, WindowSizeX(), WindowSizeY());
                button->rect.x = WindowSizeX() - 100 - button->rect.w / 2;
                button->rect.y = WindowSizeY() - 100 - button->rect.h / 2;
                flags.windowSize = CLEAR_FLAG;
            }
            if (flags.switchTurn)
            {
                MoveShip(player);
                if (selection == TURN_RIGHT)
                    TurnRight(player);
                else if (selection == TURN_LEFT)
                    TurnLeft(player);
                else if (selection == SHOOT)
                {
                    /*
                    Ship *bullet = CreateBullet(player->x, player->y, RIGHT);
                    if (bullet)
                        ArrayAppend(goodBullets, bullet);*/
                }
                ArrayIterate(goodBullets, ActivateShip);
                selection = NO_ACTION;

                // Switch turn
                turn = AI_BUFFER;
                turnTimer = SDL_GetTicks();
                flags.switchTurn = CLEAR_FLAG;
                flags.doCollision = SET_FLAG;
            }
            if (flags.doCollision)
            {
                memset(collisionHolder, NULL_INT, NumTiles() * sizeof(void*)); // Clear the array

                // Add all game objects to the array thing
                temp_collision_thing((void*) player);
                ArrayIterate(goodBullets, temp_collision_thing);
                ArrayIterate(badBullets, temp_collision_thing);
                ArrayIterate(ships, temp_collision_thing);

                for (unsigned int y = 0; y < GameField.height; y++)
                {
                    for (unsigned int x = 0; x < GameField.width; x++)
                    {
                        printf("%08p ", collisionHolder[IndexFromLocation(x, y)]);
                    }
                    printf("\n");
                }
                temp_collision_clean();
                // Kill things that are dead
                ArrayKillNonZero(badBullets, check_if_pointer_exists_in_collision, CleanupShip);
                ArrayKillNonZero(goodBullets, check_if_pointer_exists_in_collision, CleanupShip);
                ArrayKillNonZero(ships, check_if_pointer_exists_in_collision, CleanupShip);

                flags.doCollision = CLEAR_FLAG;
            }
            if (frameStartTick - turnTimer > userSettings.turnDelay)
            {
                turnTimer = 0;
                if (turn == AI_BUFFER)
                    turn = AI;
                if (turn == PLAYER_BUFFER)
                    turn = PLAYER;
                if (turn == MISC_BUFFER)
                    turn = MISC;
            }
        }

        // Drawing
        DrawField(&GameField);
        if (turn == PLAYER)
        {
            // Outline each ships next position
            for (unsigned int i = 0; i < ArrayLength(ships); i++)
            {
                if ((s = (Ship*) ArrayElement(ships, i)))
                {
                    SDL_Point point = ShipNextTile(s);
                    OutlineTile(point.x, point.y);
                }
            }
            if ((s = ArrayFind(ships, selected_ship)))
            {
                // TODO: Also display information about the ship
                SDL_SetRenderDrawColor(GameRenderer, 0xFF, 0xFF, 0x00, 0xFF);
                OutlineTileBufferColor(s->x, s->y);
                if (!selected_texture)
                {
                    char buffer[100];
                    sprintf(buffer, "%s\nAction: %s", GetNameShip(s), HumanReadableStringFrom(s->previous));
                    selected_texture = FontRenderTextSize(GameRenderer, buffer, 15, &selected_rect);
                    selected_rect.x = WindowSizeX() - selected_rect.w;
                    selected_rect.y = 200;
                    SDL_SetTextureColorMod(selected_texture, 0xFF, 0x00, 0x00);
                }
                SDL_RenderCopy(GameRenderer, selected_texture, NULL, &selected_rect);
            }
        }

        // TODO: Clean up misc stuff <- I don't know what this is referring to
        DrawShip(player);
        OutlineTile(player->x, player->y);
        if (flags.goob)
            OutlineTile(3, 1);
        ArrayIterate(ships, DrawShip);
        ArrayIterate(miscShips, DrawShip);
        ArrayIterate(badBullets, DrawShip);
        ArrayIterate(goodBullets, DrawShip);
        DrawButton(button);

        DebugDisplayDraw();


        // End of frame stuff
        SDL_RenderPresent(GameRenderer);
#ifndef UNLIMITED_FPS
        if (SDL_GetTicks() - frameStartTick <= FPS_LIMIT_THRESHOLD)
            SDL_Delay(FPS_LIMIT_RATE);
#endif // UNLIMITED_FPS
    }
    printf("Cleanup has started.\n");
    FreeShipImages();
    CleanupLibraries();
    ArrayAnnihilate(&ships, CleanupShip);
    ArrayAnnihilate(&badBullets, CleanupShip);
    ArrayAnnihilate(&goodBullets, CleanupShip);
    ArrayAnnihilate(&miscShips, CleanupShip);
    DestroyShip(player);
    return VerifyShipsFreed();
}

/** ANYTHING BELOW THIS LINE IS TEMPORARY AND SHOULD NOT REMAIN HERE **/

void temp_collision_thing(void *ship)
{
    NULL_CHECK(ship);
    // These two will almost assuredly be in registers, can't see why they wouldn't
    Ship *current = (Ship*) ship;

    size_t location = (size_t) SHIP_INDEX_FROM_LOCATION(current);
    Ship *existing = collisionHolder[location];
    if (existing)
    {
        // TODO: Need more ship functions to determine what happens when things collide
        // Ships with zero toughness will become explosions
        // The the current one at the location is tougher, then swap them so the the math isn't repeated in my code
        if (existing->toughness > current->toughness)
        {
            void **dp = (void**) &existing;
            existing = current;
            current = *dp;
        }
        // The survivor will then be reduced in toughness

        current->collision += current->toughness + existing->collision;
        if (existing->collision > current->toughness)
        {
            current->toughness = 0;
        }
        else
        {
            current->toughness -= existing->collision;
        }


    }
    else
    {
        current->collision = current->toughness;
    }
    collisionHolder[location] = current;
}

void temp_collision_clean()
{
    for (size_t index = 0; index < NumTiles(); index++)
    {
        Ship *current = (Ship*) collisionHolder[index];
        if (current && !current->toughness && current->type != EXPLOSION)
        {
            Ship *exp = CreateExplosion(current->x, current->y, RIGHT);
            if (exp)
            {
                exp->toughness = (current->collision > 0xF) ? 0xF : current->collision;
                ArrayAppend(miscShips, (void*) exp);
            }
            //printf("WE NEED AN EXPLOSION 'ERE %u %u\n", current->x, current->y);
            collisionHolder[index] = exp;
        }
    }
}

size_t check_if_pointer_exists_in_collision(void *data)
{
    NULL_CHECK_RETURN(data, 0);
    Ship *ship = (Ship*) data;
    return collisionHolder[SHIP_INDEX_FROM_LOCATION(ship)] != data;
}

// Bullet hackiness is here
void ShootGamer(Ship *ship)
{
    NULL_CHECK(ship);
    Ship *bullet = CreateBullet(ship->x, ship->y, ship->facing);
    NULL_CHECK(bullet);
    // I know this is a temp function, but you really need to always check your damn pointers
    ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
    ArrayAppend(badBullets, bullet);
}
