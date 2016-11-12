#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lval* lval_call(lenv* env, lval* fun, lval* arguments) {
  /* call builtin functions */
  if(fun->builtin) { return fun->builtin(env, arguments); }

  int given = arguments->count;
  int expected = fun->formals->count;

  /* raise error if not enough arguments were passed */
  if(given != expected) { return lval_error("Function called with non adequate number of arguments"); }

  for(int i=0; i < given; i++) {
    lval* symbol = lval_pop(fun->formals, 0);
    lval* value  = lval_pop(arguments, 0);

    lenv_put(fun->env, symbol, value);

    lval_delete(symbol);
    lval_delete(value);
  }

  fun->env->parent = env;

  lval* code = lval_add(lval_sexpr(), lval_copy(fun->body));

  return builtin_eval(fun->env, code);
}

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

  lval* result = lval_call(env, f, value);
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

lval* lispy_eval(mpc_parser_t* parser, lenv* env, char* code) {
  mpc_result_t parsing_result;

  lval* result;

  if(mpc_parse("<stdin>", code, parser, &parsing_result)) {
    result = lval_eval(env, lval_read(parsing_result.output));

    mpc_ast_delete(parsing_result.output);
  } else {
    result = lval_error("Incorect syntaxt");

    mpc_err_delete(parsing_result.error);
  }

  return result;
}
