#include "array.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct Array
{
    void **array;
    unsigned int length;
    unsigned int size;
} Array;

Array* ArrayCreate(unsigned int size)
{
    Array* array = (Array*) calloc(1, sizeof(Array));
    if (array)
    {
        array->size = exp2(ceil(log2(size ? 1 : size)));
        array->array = (void**) calloc(size, sizeof(void*) * array->size);
        array->length = 0;
    }
    return array;
}

unsigned int ArrayLength(Array *array)
{
    return array ? array->length : 0;
}

unsigned int ArraySize(Array *array)
{
    return array ? array->size : 0;
}

void ArrayDestroy(Array *array)
{
    if (array)
    {
        free(array->array);
        free(array);
    }
}

void ArrayPurge(Array **array)
{
    if (array && *array)
    {
        ArrayIterate(*array, free);
        ArrayDestroy(*array);
        *array = NULL;
    }
}

void ArrayAdd(Array *array, unsigned int index, void *data)
{
    if (!array || !data)
        return;
    if (array->size <= index)
    {
        // Resize array
        array->size *= 2;
        array->array = realloc(array->array, array->size * sizeof(void*));
    }
    array->array[index] = data;
    array->length++;
}

void ArrayAppend(Array *array, void *data)
{
    if (array)
        ArrayAdd(array, array->length, data);
}

void ArrayIterate(Array *array, ArrayFunc func)
{
    if (!array || !func)
        return;
    for (unsigned int i = 0; i < array->length; i++)
        func(array->array[i]);
}

void *ArrayElement(Array *array, unsigned int index)
{
    if (!array)
        return NULL;
    assert(array->length > index);
    return (void*)array->array[index];
}

void **ArrayRemove(Array *array, unsigned int index)
{
    return ArrayRemoveRange(array, index, index);
}

void **ArrayRemoveRange(Array *array, unsigned int start, unsigned int end)
{
    static void **pointers = NULL;
#ifndef UNSAFE_ARRAYS
    if (!array || array->length < start || array->length < end || start > end)
        return NULL;
#endif //UNSAFE_ARRAYS
    int index = end - start + 1;
    pointers = realloc(pointers, (index + 1) * sizeof(void*));
    if (pointers)
    {
        memcpy(pointers, array->array + start, index * sizeof(void*));
        pointers[index] = NULL;
    }
    memmove(array->array + start, array->array + start + index, (array->length - index) * sizeof(void*));
    array->length -= index;
    return pointers;
}

void ArrayDeleteRange(Array *array, unsigned int start, unsigned int end)
{
    if (array)
    {
        void** sub = ArrayRemoveRange(array, start, end);
        for (; sub || *sub; sub++)
            free(*sub);
    }
}

void* ArrayPop(Array *array)
{
    void *value = NULL;
    if (array && array->array)
    {
        void **data = ArrayRemove(array, array->length - 1);
        if (data)
            value = data[0];
    }
    return value;
}

void ArrayDelete(Array *array, void *data)
{
    if (!array || !data)
        return;
    void **location = array->array;
    while (location != array->array + array->length + 1)
    {
        if (*location == data)
        {
            ArrayRemove(array, (int) (array->array - location));
            return;
        }
        location++;
    }
}

void ArrayClear(Array *array, ArrayFunc clean)
{
    if (array)
    {
        ArrayIterate(array, clean);
        array->length = 0;
    }
}
