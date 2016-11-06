#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lval* builtin_plus(lenv* env, lval* value) {
  long result = 0;

  for(int i=0; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_NUM) {
      return lval_error("+ cannot operate on non-number!");
    }

    result += value->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_minus(lenv* env, lval* value) {
  if(value->cell[0]->type != LVAL_NUM) {
    return lval_error("- cannot operate on non-number!");
  }

  if(value->count == 1) {
    return lval_number(-value->cell[0]->number);
  }

  long result = value->cell[0]->number;

  for(int i=1; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_NUM) {
      return lval_error("- cannot operate on non-number!");
    }

    result -= value->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_times(lenv* env, lval* value) {
  long result = 1;

  for(int i=0; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_NUM) {
      return lval_error("* cannot operate on non-number!");
    }

    result *= value->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_divide(lenv* env, lval* value) {
  long result = value->cell[0]->number;

  for(int i=1; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_NUM) {
      return lval_error("/ cannot operate on non-number!");
    }

    if(value->cell[i]->number == 0) {
      return lval_error("Division By Zero!");
    }

    result /= value->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_list(lenv* env, lval* value) {
  lval* result = lval_qexpr();
  result->count = value->count;
  result->cell = value->cell;

  value->count = 0;
  value->cell = NULL;

  return result;
}

lval* builtin_head(lenv* env, lval* value) {
  if(value->count != 1)                  { return lval_error("Function head expects one argument"); }
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function head called with incorect datatype"); }
  if(value->cell[0]->count == 0)         { return lval_error("Function head called on empty list"); }

  return lval_pop(value->cell[0], 0);
}

lval* builtin_tail(lenv* env, lval* value) {
  if(value->count != 1)                  { return lval_error("Function tail expects one argument"); }
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function tail called with incorect datatype"); }
  if(value->cell[0]->count == 0)         { return lval_error("Function tail called on empty list"); }

  lval* first_argument = lval_pop(value, 0);
  lval* first_element = lval_pop(first_argument, 0);

  lval_delete(first_element);

  return first_argument;
}

lval* builtin_join(lenv* env, lval* value) {
  if(value->count == 0) { return lval_error("Function join expects at least one argument"); }

  for (int i = 0; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_QEXPR) { return lval_error("Function join called with wrong datatype"); }
  }

  lval* result = lval_pop(value, 0);

  while(value->count) {
    result = lval_join(result, lval_pop(value, 0));
  }

  return result;
}

lval* builtin_eval(lenv* env, lval* value) {
  if(value->count != 1)         { return lval_error("Function eval expects one argument"); }
  if(value->type == LVAL_QEXPR) { return lval_error("Function eval called with incorect datatype"); }

  lval* result = lval_pop(value, 0);
  result->type = LVAL_SEXPR;

  return lval_eval(env, result);
}