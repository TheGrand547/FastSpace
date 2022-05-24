#include "array.h"
#include <assert.h>
#include <math.h>
#include <string.h>

typedef struct Array
{
    void **array;
    size_t length;
    unsigned int size;
} Array;

Array* ArrayCreate(size_t size)
{
    Array* array = (Array*) calloc(1, sizeof(Array));
    if (array)
    {
        array->size = exp2(ceil(log2(size ? size : 1)));
        array->array = (void**) calloc(size, sizeof(void*) * array->size);
        array->length = 0;
    }
    return array;
}

size_t ArrayLength(Array *array)
{
    return array ? array->length : 0;
}

size_t ArraySize(Array *array)
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

void ArrayAnnihilate(Array **array, ArrayFunc clean)
{
    if (array && *array)
    {
        ArrayIterate(*array, clean);
        ArrayDestroy(*array);
        *array = NULL;
    }
}

void ArrayInsert(Array *array, size_t index, void *data)
{
    if (!array)
        return;
    if (index >= ArraySize(array))
        ArrayAppend(array, data);
    else
    {
        memmove(array->array + index + 1, array->array + index, (array->length - index) * sizeof(void*));
        array->array[index] = data;
        array->length++;
    }
}

static void ArrayAdd(Array *array, size_t index, void *data)
{
    if (!array || !data)
        return;
    if (array->size <= index)
    {
        // Resize array; averages out to be constant time for rapid reallocations
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
    for (size_t i = 0; i < array->length; i++)
        func(array->array[i]);
}

void **ArrayReference(Array *array, size_t index)
{
    if (!array)
        return NULL;
    return (void**) array->array + index;
}

// Feel this like could somehow be wrapped into a call to ArrayReference but that might be a bad idea
void *ArrayElement(Array *array, size_t index)
{
    if (!array)
        return NULL;
    return (void*)array->array[index];
}

void **ArrayRemove(Array *array, size_t index)
{
    return ArrayRemoveRange(array, index, index);
}

void **ArrayRemoveRange(Array *array, size_t start, size_t end)
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

void ArrayDeleteRange(Array *array, size_t start, size_t end)
{
    if (array)
    {
        void** sub = ArrayRemoveRange(array, start, end);
        for (; sub || *sub; sub++)
            free(*sub);
    }
}

void *ArrayPop(Array *array)
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
