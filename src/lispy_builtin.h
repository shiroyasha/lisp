#pragma once

#include "lispy.h"

lval* builtin_plus(lenv* env, lval* value);
lval* builtin_minus(lenv* env, lval* value);
lval* builtin_times(lenv* env, lval* value);
lval* builtin_divide(lenv* env, lval* value);

lval* builtin_list(lenv* env, lval* value);
lval* builtin_head(lenv* env, lval* value);
lval* builtin_tail(lenv* env, lval* value);
lval* builtin_join(lenv* env, lval* value);
lval* builtin_eval(lenv* env, lval* value);

lval* builtin_def(lenv* env, lval* value);
