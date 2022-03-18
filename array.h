#pragma once
#ifndef ARRAY_H
#define ARRAY_H

struct Array;
typedef struct Array Array;
typedef void(*ArrayFunc)(void*);
typedef int(*ArrayCriteria)(void*);

Array* ArrayCreate(unsigned int size);
Array* ArrayCreate(unsigned int size);
void ArrayClear(Array *array, ArrayFunc clean);

#define ArrayNew() ArrayCreate(1)
#define ArrayClean(array) ArrayClear(array, free)

unsigned int ArrayLength(Array *array);
unsigned int ArraySize(Array *array);

void ArrayPurge(Array **array);
void ArrayAdd(Array *array, unsigned int index, void *data);
void ArrayInsert(Array *array, unsigned int index, void *data);
void ArrayAppend(Array *array, void *data);
void ArrayDeleteRange(Array *array, unsigned int start, unsigned int end);
void **ArrayRemove(Array *array, unsigned int index);
void *ArrayPop(Array *array);
void **ArrayRemoveRange(Array *array, unsigned int start, unsigned int end);
void *ArrayElement(Array *array, unsigned int index);
void ArrayIterate(Array *array, ArrayFunc func);
#endif // ARRAY_H
