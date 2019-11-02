#ifndef UTILC_SCOPE_H
#define UTILC_SCOPE_H

#include <stdlib.h>

/** Macro to create a Scope named scope_ */
#define Scope ScopeType scope_ = {0};

/** Macro to add a data to be freed to the Scope scope_ */
#define ScopeAddData(pointer_to_data) scope_add_data(&scope_, (void**) (pointer_to_data));

/** Macro to remove added date from the Scope scope_ */
#define ScopeRemoveData(data) scope_remove_data(&scope_, (void*) (data));

/**
 * Macro to add a a call to a void(void*) function like a destructor or fclose to the Scope scope_
 * e. g.:
 * void Car_kill(Car *self) { ... }
 */
#define ScopeAddCall(object, function) scope_add_call(&scope_, (void*) (object), (void (*)(void*)) (function));

/** Macro to remove a call from the Scope scope_ */
#define ScopeRemoveCall(object) scope_remove_call(&scope_, (void*) object);

/** Macro to free all added data and call each function call with the associated object */
#define ScopeLeave scope_leave(&scope_);

/**
 * Macro to free all added data and call each function call with the associated object.
 * Also returns from the actual function with the given return value (use {} to return void)
 */
#define ScopeLeaveReturn(to_return) { scope_leave(&scope_); return (to_return); }


/** The data used to store the current Scope */
typedef struct {
    void ***pointer_to_data_array;
    int pointer_to_data_array_size;

    void (**object_function_array)(void *);

    void **object_array;
    int object_array_size;
} ScopeType;


/** Add a data to be freed to the Scope self */
static void scope_add_data(ScopeType *self, void **pointer_to_data) {
    self->pointer_to_data_array = (void ***) realloc(self->pointer_to_data_array,
                                                     sizeof(void **) * ++self->pointer_to_data_array_size);
    self->pointer_to_data_array[self->pointer_to_data_array_size - 1] = pointer_to_data;
}

/** Remove added date from the Scope self */
static void scope_remove_data(ScopeType *self, void *data) {
    for (int i = 0; i < self->pointer_to_data_array_size; i++) {
        if (data == *self->pointer_to_data_array[i])
            self->pointer_to_data_array[i] = NULL;
    }
}

/**
 * Add a a call to a void(void*) function like a destructor or fclose to the Scope self
 * e. g.:
 * void Car_kill(Car *self) { ... }
 */
static void scope_add_call(ScopeType *self, void *object, void (*function)(void *)) {
    self->object_array = (void **) realloc(self->object_array, sizeof(void *) * ++self->object_array_size);
    self->object_function_array = (void (**)(void *)) realloc(self->object_function_array,
                                                              sizeof(void (*)(void *)) * self->object_array_size);
    self->object_array[self->object_array_size - 1] = object;
    self->object_function_array[self->object_array_size - 1] = function;
}

/** Remove a call from the Scope self */
static void scope_remove_call(ScopeType *self, void *object) {
    for (int i = 0; i < self->object_array_size; i++) {
        if (object == self->object_array[i])
            self->object_array[i] = NULL;
    }
}

/** Free all added data and call each function call with the associated object */
static void scope_leave(ScopeType *self) {
    for (int i = self->pointer_to_data_array_size - 1; i >= 0; i--) {
        if (self->pointer_to_data_array[i])
            free(*self->pointer_to_data_array[i]);
    }
    free(self->pointer_to_data_array);
    self->pointer_to_data_array_size = 0;

    for (int i = self->object_array_size; i >= 0; i--) {
        if (self->object_array[i])
            self->object_function_array[i](self->object_array[i]);
    }
    free(self->object_function_array);
    free(self->object_array);
    self->object_array_size = 0;
}

#endif //UTILC_SCOPE_H
