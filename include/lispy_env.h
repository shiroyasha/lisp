#pragma once

#include "lispy.h"

lenv* lenv_new(void);
lenv* lenv_copy(lenv* e);
lval* lenv_get(lenv* env, lval* key);

void lenv_put(lenv* env, lval* key, lval* value);
void lenv_delete(lenv* e);
void lenv_add_builtin(lenv* e, char* name, lbuiltin fun);

void lenv_print(lenv* e);
