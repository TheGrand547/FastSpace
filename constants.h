#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Bunch of defines that determine various factors
// Big one is RELEASE, which if defined turns off all random misc things,
// in particular most textual output. It's done automatically via my IDE but here for convience
//#define RELEASE

//#define UNLIMITED_FPS
#define FPS_LIMIT_THRESHOLD 5 // Threshold for the below
#define FPS_LIMIT_RATE 2 // Ms to SDL_Wait() per frame if it took fewer ms than FPS_LIMIT_THRESHOLD


#endif // CONSTANTS_H
