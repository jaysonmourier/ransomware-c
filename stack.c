#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stack.h"

static void **resize_stack_memory(struct Stack *stack, size_t new_capacity) {
    if(stack == NULL) {
        return NULL;
    }

    if(new_capacity < stack->size) {
        printf("[ERROR] resize_stack_memory: New capacity (%zu) is too small for the current stack size (%zu).\n", new_capacity, stack->size);
        return NULL;
    }

    void **resized_data = realloc(stack->data, new_capacity * sizeof(void *));
    if(resized_data == NULL) {
        return NULL;
    }

    stack->capacity = new_capacity;
    stack->data = resized_data;

    return resized_data;
}

struct Stack *stack_init() {
    struct Stack *stack = calloc(1, sizeof(struct Stack));
    if(stack == NULL) {
        printf("[ERROR STACK INIT] Unable to allocate memory\n");
        return NULL;
    }

    stack->capacity = INITIAL_STACK_CAPACITY;
    stack->size = 0;

    stack->data = calloc(stack->capacity, sizeof(void*));
    if(stack->data == NULL) {
        printf("[ERROR STACK INIT] Unable to allocate memory for data\n");
        free(stack);
        return NULL;
    }

    return stack;
}

void stack_free(struct Stack **stack, void (*fn)(void *)) {
    if(stack && *stack) {
        if(fn != NULL) {
            for(size_t i = 0; i < (*stack)->size; ++i) {
                fn((*stack)->data[i]);
            }
        }
        free((*stack)->data);
        free(*stack);
        *stack = NULL;
    }
}

void stack_push(struct Stack *stack, void *element) {
    if(stack == NULL) {
        printf("[ERROR] stack_push: La pile n'existe pas !\n");
        return;
    }

    if(stack->size == stack->capacity) {
        size_t new_capacity = (stack->capacity == 0) ? INITIAL_STACK_CAPACITY : stack->capacity * 2;
        if(resize_stack_memory(stack, new_capacity) == NULL) {
            printf("[ERROR] stack_push: Impossible d'étendre la pile !\n");
            return;
        }
    }

    stack->data[stack->size] = element;

    stack->size += 1;
}

void *stack_pop(struct Stack *stack) {
    if(stack == NULL || stack_is_empty(stack)) {
        return NULL;
    }

    void *element = stack->data[stack->size - 1];
    stack->data[stack->size - 1] = NULL;
    stack->size -= 1;

    if(stack->size < (stack->capacity / 4) && stack->capacity > INITIAL_STACK_CAPACITY) {
        size_t new_capacity = (stack->capacity / 2 > stack->size) ? stack->capacity / 2 : stack->size;
        new_capacity = (new_capacity < INITIAL_STACK_CAPACITY) ? INITIAL_STACK_CAPACITY : new_capacity;

        if(resize_stack_memory(stack, new_capacity) == NULL) {
            printf("[WARNING STACK POP] Unable to realloc memory\n");
        }
    }

    return element;
}

void *stack_peek(struct Stack *stack) {
    return ((stack != NULL) && !stack_is_empty(stack)) ? stack->data[stack->size - 1] : NULL;
}

size_t stack_size(struct Stack *stack) {
    return (stack != NULL) ? stack->size : 0;
}

bool stack_is_empty(struct Stack *stack) {
    return (stack == NULL) || (stack->size == 0);
}

void stack_clear(struct Stack *stack, void (*fn)(void*)) {
    if((stack != NULL) && !stack_is_empty(stack)) {
        if(fn != NULL) {
            for(size_t i = 0; i < stack->size; ++i) {
                fn(stack->data[i]);
            }
        }
        
        stack->size = 0;
    }
}
