#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct Linked;
typedef struct Linked Linked;

typedef void (*LinkedFunc)(void*);
typedef int (*LinkedCriteriaFunc)(void*);

void LinkedInsert(Linked *linked, void *data);
void LinkedIterate(Linked *linked, LinkedFunc func);
void LinkedCriteriaRemove(Linked *linked, LinkedCriteriaFunc func);
void LinkedRemove(Linked *linked, void *data);

#endif // LINKED_LIST_H
