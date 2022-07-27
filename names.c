#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "super_header.h"

static const char *faction_strings[] = {
    "Goober", "Scoober", "Canoodler", "Hoplite"
};

// TODO: Will need up to 255 of them
static const char *captain_names[] = {
    "Sam", "Ryan", "Jack", "Jill", "Elif", "Skipper"
};
#define NAME_LENGTH STATIC_ARRAY_LENGTH(captain_names)
#define FACTION_LENGTH STATIC_ARRAY_LENGTH(faction_strings)

static size_t faction_index = 0;
static size_t captain_index = 0;

void SetFactionIndex(const size_t index)
{
    faction_index = index;
}

char *GetName(const char *type)
{
    const char *faction = faction_strings[faction_index];
    const char *captain = captain_names[captain_index++];
    // 3 = 2 spaces + 1 null byte
    char *buffer = malloc(strlen(faction) + strlen(type) + strlen(captain) + 3);
    if (buffer)
        sprintf(buffer, "%s %s %s", faction, type, captain);
    return buffer;
}
