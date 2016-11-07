#pragma once

#include "lispy.h"

/* constructors */

lval* lval_number(long number);
lval* lval_error(char* format, ...);
lval* lval_symbol(char* symbol);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_builtin(lbuiltin fun);
lval* lval_lambda(lval* formals, lval* body);

/* destructor */

void lval_delete(lval* value);

/* printing */

void lval_print(lval* value);
void lval_print(lval* value);
void lval_println(lval* v);

/* operations */

lval* lval_pop(lval* value, int index);
lval* lval_take(lval* value, int index);
lval* lval_add(lval* a, lval* b);
lval* lval_join(lval* a, lval* b);
lval* lval_copy(lval* a);
