#ifndef H_ALGO_STACK
#define H_ALGO_STACK

#include <stddef.h>
#include <stdbool.h>

#define INITIAL_STACK_CAPACITY 1000

struct Stack {
    size_t capacity; 
    size_t size; 
    void **data; 
}; 

struct Stack *stack_init();
void stack_free(struct Stack **stack, void (*fn)(void *));
void stack_push(struct Stack *stack, void *element);
bool stack_is_empty(struct Stack *stack);
void *stack_pop(struct Stack *stack);
#endif
