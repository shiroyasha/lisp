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
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function eval called with incorect datatype"); }

  lval* code = lval_pop(value, 0);
  code->type = LVAL_SEXPR;

  lval* result = lval_eval(env, code);

  return result;
}

lval* builtin_def(lenv* env, lval* value) {
  if(value->cell[0]->type != LVAL_QEXPR)  { return lval_error("Function def called with incorect datatype"); }

  lval* symbol_list = lval_pop(value, 0);

  /* ensure that all elements of the symbol list are symbols */
  for(int i=0; i < symbol_list->count; i++) {
    if(symbol_list->cell[i]->type != LVAL_SYM) {
      lval_delete(symbol_list);
      return lval_error("Function def cannot define non-symbol");
    }
  }

  /* ensure that the number of arguments is the same as the number of symbols */
  if(symbol_list->count < value->count) { return lval_error("Function def received less symbols than values"); }
  if(symbol_list->count > value->count) { return lval_error("Function def received more symbols than values"); }

  for(int i=0; i < symbol_list->count; i++) {
    lenv_put(env, symbol_list->cell[i], value->cell[i]);
  }

  return lval_sexpr();
}

lval* builtin_lambda(lenv* env, lval* value) {
  if(value->count != 2)                  { return lval_error("Function \\ expects exactly two arguments"); }
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function \\ expects its first argument to be a Q-Expression"); }
  if(value->cell[1]->type != LVAL_QEXPR) { return lval_error("Function \\ expects its second argument to be a Q-Expression"); }

  lval* symbol_list = lval_pop(value, 0);

  /* ensure that all elements of the symbol list are symbols */
  for(int i=0; i < symbol_list->count; i++) {
    if(symbol_list->cell[i]->type != LVAL_SYM) {
      lval_delete(symbol_list);
      return lval_error("Parameter list must contain only symbols");
    }
  }

  lval* body = lval_pop(value, 0);

  return lval_lambda(symbol_list, body);
}

lval* builtin_do(lenv* env, lval* value) {
  if(value->count == 0) { return lval_error("Function do expects at least one argument"); }

  for (int i = 0; i < value->count; i++) {
    if(value->cell[i]->type != LVAL_QEXPR) { return lval_error("Function do called with wrong datatype"); }
  }

  lval* result = NULL;

  while(value->count) {
    lval* block = lval_pop(value, 0);
    block->type = LVAL_SEXPR;

    result = lval_eval(env, block);
  }

  return result;
}
