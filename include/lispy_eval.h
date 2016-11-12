#pragma once

#include "lispy.h"

lval* lval_eval(lenv* env, lval* value);
lval* lispy_eval(mpc_parser_t* parser, lenv* env, char* code);
