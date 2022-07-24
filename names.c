#include <string.h>
#include <stdio.h>

static const char *faction_strings[] = {
    "Goober", "Scoober", "Canoodler", "Hoplite"
};

// TODO: Will need up to 255 of them
static const char *captain_names[] = {
    "Sam", "Ryan", "Jack", "Jill", "Elif", "Skipper"
};
#define NAME_LENGTH sizeof(captain_names) / sizeof(const char*)

static size_t faction_index = 0;

void SetFactionIndex(const size_t index)
{
    faction_index = index;
}

char *GetName(const char *type)
{
    char buffer[100];
    // TODO: The shift thingy is cringe don't do it
    sprintf(buffer, "%s %s %s", faction_strings[faction_index], type, captain_names[(((size_t) type) >> 3) % NAME_LENGTH]);
    return strdup(buffer);
}
