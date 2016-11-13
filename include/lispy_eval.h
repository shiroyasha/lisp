#pragma once

#include "lispy.h"

lval* lval_eval(lenv* env, lval* value);
lval* lispy_eval(lparser* parser, lenv* env, char* code);
