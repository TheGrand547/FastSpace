#include "array.h"
#include <assert.h>
#include <math.h>
#include <string.h>

typedef struct Array
{
    void **array;
    size_t length;
    size_t size;
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

// Returns zero if pointer is null, otherwise non-zero
size_t null_non_zero(void *data)
{
    return (size_t) data;
}

// Empty 'free'
void dummy(void *data)
{
    (void) data;
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

void ArrayClear(Array *array)
{
    if (!array && array->array)
        return;
    ArrayClearWithoutResize(array);
    array->length = 0;
}

void ArrayClearWithoutResize(Array *array)
{
    if (!array && array->array)
        return;
    memset(array->array, 0, array->size * sizeof(void*));
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
    if (index >= array->size)
        ArrayAppend(array, data);
    else
    {
        // TODO: Verify location
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
    if (!array || array->length >= index)
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
    // Want to move the last n elements back index number of steps,
    // where n = len - the last location removed
    memmove(array->array + start, array->array + start + index, (array->length - end) * sizeof(void*));
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

void *ArrayFind(Array *array, void *element)
{
    if (!array || !element)
        return NULL;
    size_t size = array->length;
    if (size > 0) {
        for (size_t index = 0; index < size; index++) {
            if (array->array[index] == element)
                return element;
        }
    }
    return NULL;
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

void ArrayReserve(Array *array, size_t size)
{
    if (!array || array->size >= size)
        return;
    size_t elements = exp2(ceil(log2(size ? size : 1)));
    array->array = realloc(array->array, elements * sizeof(void*));
    memset(array->array + array->length, 0, elements - array->length);
    array->size = size;
}

void ArrayCleanup(Array *array, ArrayFunc clean)
{
    if (array)
    {
        ArrayIterate(array, clean);
        array->length = 0;
    }
}

void ArrayKillNonZero(Array *array, ArrayCriteria criteria, ArrayFunc cleanup)
{
    if (!array || !array->array)
        return;
    size_t index = 0;
    for (size_t i = 0; i < array->length; i++)
    {
        void *element = array->array[i];
        if (!criteria(element))
        {
            array->array[index++] = element;
        }
        else
        {
            cleanup(element);
        }
    }
    array->length = index;
}
