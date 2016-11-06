#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lval* lval_eval_sexpr(lenv* env, lval* value) {
  /* evaluate every child */
  for(int i=0; i < value->count; i++) {
    value->cell[i] = lval_eval(env, value->cell[i]);
  }

  /* check for errors */
  for(int i=0; i < value->count; i++) {
    if(value->type == LVAL_ERR) { return lval_take(value, i); }
  }

  /* empty expression */
  if(value->count == 0) { return value; }

  /* single value expression */
  if(value->count == 1) { return lval_take(value, 0); }

  /* ensure that first element is a symbol */
  lval* f = lval_pop(value, 0);
  if(f->type != LVAL_FUN) {
    lval_delete(value);
    lval_delete(f);
    return lval_error("First value is not a function");
  }

  lval* result = f->builtin(env, value);
  lval_delete(f);
  lval_delete(value);
  return result;
}

lval* lval_eval(lenv* env, lval* value) {
  if(value->type == LVAL_SYM) {
    lval* x = lenv_get(env, value);
    lval_delete(value);
    return x;
  }

  if(value->type == LVAL_SEXPR) { return lval_eval_sexpr(env, value); }

  return value;
}
