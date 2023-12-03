#include "cstack.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


// *** LIBRARY SETTINGS ***

#define STACK_TABLE_HANDLER 12
#define UNUSED(VAR) (void)(VAR)

// ========================

#pragma pack(1)
typedef struct node 
{
    void*                   data;
    struct node*            prev;
    unsigned int            size;
} node_t;
#pragma pack()

#pragma pack(1)
typedef struct stack
{
    struct node*            entry;
} stack_t;
#pragma pack()

#pragma pack(1)
typedef struct stack_entries_table 
{
    stack_t*                stacks[STACK_TABLE_HANDLER];
    int                     count;
} stack_entries_table_t;
#pragma pack()

stack_entries_table_t g_table = {.stacks = NULL, .count = 0 };

hstack_t stack_new(void)
{
    if(g_table.count == STACK_TABLE_HANDLER - 1)
    {
        return -1;
    }

    stack_t* _stack =        (stack_t*)malloc(sizeof(stack_t));

    if (_stack == NULL) 
    {
        return -1;
    }

    _stack->entry = NULL;
    g_table.stacks[g_table.count] = _stack;
    
    return g_table.count++;
}

void stack_free(const hstack_t hstack)
{
    if (stack_valid_handler(hstack))
    {
        return;
    }

    stack_t* _ptrStack = g_table.stacks[hstack];

    if (_ptrStack->entry != NULL) 
    {
        node_t* _nextTopNode = (node_t*)NULL;
        for (int _i = 0, _s = stack_size(hstack); _i < _s; ++_i)
        {
            _nextTopNode = _ptrStack->entry->prev;

            free(_ptrStack->entry->data);
            free(_ptrStack->entry);

            _ptrStack->entry = _nextTopNode;
        }

        free(_ptrStack->entry);
    }

    free(_ptrStack);
    g_table.stacks[hstack] = (stack_t*)NULL;
    --g_table.count;
}

int stack_valid_handler(const hstack_t hstack)
{
    if ((hstack < 0) ||

        (hstack >= STACK_TABLE_HANDLER) ||
        
        (g_table.stacks[hstack] == NULL)) 
    {
        return 1;
    }
    return 0;
}

unsigned int stack_size(const hstack_t hstack)
{
    if (stack_valid_handler(hstack))
    {
        return 0;
    }

    int _i = 0;

    for (node_t* _node = g_table.stacks[hstack]->entry; _node != (node_t*)NULL; _node = _node->prev) 
    {
        ++_i;
    }
    return _i;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if  ((stack_valid_handler(hstack)) ||

        (data_in == NULL || size == 0))
    {
        return;
    }

    node_t* _ptr = (node_t*)malloc(sizeof(node_t));
    
    if (_ptr == NULL) 
    {
        return;
    }
   
    _ptr->size = size;
    _ptr->data = malloc(size);
    
    if (_ptr->data == NULL) 
    {
        free(_ptr);
        return;
    }

    memcpy_s(_ptr->data, _ptr->size, data_in, size);
    _ptr->prev = g_table.stacks[hstack]->entry;
    g_table.stacks[hstack]->entry = _ptr;
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if  ((stack_valid_handler(hstack)) ||

        (data_out == NULL || size == 0) ||

        (g_table.stacks[hstack]->entry == NULL) ||

        (g_table.stacks[hstack]->entry->size > size))
    {
        return 0;
    }

    memcpy_s(data_out, size, g_table.stacks[hstack]->entry->data, g_table.stacks[hstack]->entry->size);

    node_t* _nextTopNode = g_table.stacks[hstack]->entry->prev;

    free(g_table.stacks[hstack]->entry->data);
    free(g_table.stacks[hstack]->entry);

    g_table.stacks[hstack]->entry = _nextTopNode;
    return size;
}

