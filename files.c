#include "files.h"
#include <stdio.h>
#include <stdlib.h>

/* Single data point, no need to parallelize */
FileData LoadFile(char *name)
{
    FileData result = (FileData) {NULL, 0};
    FILE *file = fopen(name, "r");
    if (file)
    {
        size_t size;
#ifdef SEEK_END
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
        result.data = (void*)(char*) calloc(size, sizeof(char));
        if (result.data)
            result.amount = fread(result.data, sizeof(char), size, file);
#else  // SEEK_END
        fprintf(STDERR, "Non trivial full file reading not yet implemented due to being stupid\n");
#endif // SEEK_END
    }
    fclose(file);
    return result;
}
