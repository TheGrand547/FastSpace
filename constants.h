#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

//#define UNLIMITED_FPS
#define FPS_LIMIT_THRESHOLD 5 // Threshold for the below
#define FPS_LIMIT_RATE 2 // Ms to SDL_Wait() per frame if it took fewer ms than FPS_LIMIT_THRESHOLD
#endif // CONSTANTS_H
