#include "linked.h"
#include <stdlib.h>

typedef struct __node
{
    struct __node *next, *prev;
    void *data;
} Node;

typedef struct Linked
{
    Node *first, *last;
    int length;
} Linked;

void LinkedInsert(Linked *list, void *data)
{
    if (!list || !data)
        return;
    Node *next = calloc(1, sizeof(Node));
    if (next)
    {
        next->data = data;
        next->next = NULL;
        next->prev = list->last;
        list->last->next = next;
        list->last = next;
        if (!list->first)
            list->first = next;
        list->length++;
    }
}

void LinkedIterate(Linked *linked, LinkedFunc func)
{
    if (!linked || !func)
        return;
    Node *current = linked->first;
    while (current)
    {
        func(current->data);
        current = current->next;
    }
}

static void _linkedRemove(Linked *linked, Node *node)
{
    if (node && linked)
    {
        if (die == linked->first)
            linked->first = die->next;
        if (die == linked->last)
            linked->first = die->prev;
        linked->length--;
        if (die->prev)
            die->prev->next = die->next;
        if (die->next)
            die->next->prev = die->prev;
        free(die->data);
    }
}

void LinkedRemove(Linked *linked, void *pointer)
{
    if (!linked || !pointer)
        return;
    Node *die = linked->first;
    while (die->data != pointer && die->next)
        die = die->next;
    if (die->data == pointer)
    {
        free(linked, die->data);
        _linkedRemove(die);
    }
}

void LinkedCriteriaRemove(Linked *linked, LinkedCriteriaFunc func)
{
    if (!linked || !pointer)
        return;
    Node *current = linked->first, *next;

    while (current)
    {
        next = current->next
        if (func(current))
        {
            free(current->data);
            _linkedRemove(linked, current);
            current = next;
        }
        else
        {
            current = current->next;
        }
    }
}

void LinkedRemoveSublist(Linked *linked, Linked *rem)
{
    if (!linked || !rem || linked->size < rem)
        return;
    Node *current = linked->first;
    while (current)
    {
        Node *internal = rem->first;
        while (internal)
        {
            if (current->data == internal->data)
            {
                free(current->data);
                _linkedRemove(linked, current);
                _linkedRemove(rem, internal);
            }
            else
            {
                internal = internal->next;
            }
        }
    }
}

