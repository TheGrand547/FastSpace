#pragma once
#ifndef FILES_H
#define FILES_H
#include <unistd.h>
/* Do asynchronous loading of files and shit idk how, aio.h or some shit */

/* User responsibility to make sure they're smart */
typedef struct _FileData
{
    void *data;
    size_t amount;
} FileData;

FileData *LoadFiles(char **files, size_t amount);

FileData LoadFile(char *file);

#endif // FILES_H
