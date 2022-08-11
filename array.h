#pragma once
#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>

struct Array;
typedef struct Array Array;
typedef void(*ArrayFunc)(void*);
typedef int(*ArrayCriteria)(void*);

Array* ArrayCreate(size_t size);
void ArrayCleanup(Array *array, ArrayFunc clean);
void ArrayAnnihilate(Array **array, ArrayFunc clean);
#define ArrayPurge(array) ArrayAnnihilate(array, free)

#define ArrayNew() ArrayCreate(1)

size_t ArrayLength(Array *array);
size_t ArraySize(Array *array);
void ArrayReserve(Array *array, size_t size);
void ArrayInsert(Array *array, size_t index, void *data);
void ArrayAppend(Array *array, void *data);
void ArrayClear(Array *array);
void ArrayClearWithoutResize(Array *array);
void ArrayDeleteRange(Array *array, size_t start, size_t end);
void **ArrayRemove(Array *array, size_t index);
void **ArrayReference(Array *array, size_t index);
void *ArrayPop(Array *array);
void **ArrayRemoveRange(Array *array, size_t start, size_t end);
void *ArrayElement(Array *array, size_t index);
void *ArrayFind(Array *array, void *element);
void ArrayIterate(Array *array, ArrayFunc func);
#endif // ARRAY_H
