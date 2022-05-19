#include "debug_display.h"
#include <stdio.h>

static Uint8 InitializedDisplays;
static DisplayLocation DisplaysPlace[] = {TOP_RIGHT, TOP_RIGHT, TOP_RIGHT};

void InitDebugDisplay(char **argv)
{
    InitializedDisplays = 0;
    for (; *argv ; argv++)
        printf("%s\n", *argv);
}

static void EnableSpecificDebug(DebugDisplayFlags flags);

void EnableDebugDisplay(Uint8 flags)
{
    for (unsigned int i = 0; i < 8; i++)
    {
        if (flag | )
    }
}
