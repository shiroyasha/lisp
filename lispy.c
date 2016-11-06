#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "src/lispy.h"

void evaluate(mpc_parser_t* parser, lenv* env, char* code) {
  mpc_result_t result;

  if(mpc_parse("<stdin>", code, parser, &result)) {
    lval* value = lval_eval(env, lval_read(result.output));
    lval_delete(value);
    mpc_ast_delete(result.output);
  } else {
    printf("error\n");
    exit(1);
  }
}

void define_stdlib(mpc_parser_t* parser, lenv* env) {
  evaluate(parser, env, "def {fun} (\\ {args body} {def (list (head args)) (\\ (tail args) body)})");
}

int main(int argc, char **argv) {
  mpc_parser_t* Number      = mpc_new("number");
  mpc_parser_t* Symbol      = mpc_new("symbol");
  mpc_parser_t* SExpression = mpc_new("sexpr");
  mpc_parser_t* QExpression = mpc_new("qexpr");
  mpc_parser_t* Expression  = mpc_new("expr");
  mpc_parser_t* Lispy       = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                  \
    number : /-?[0-9]+/ ;                              \
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;        \
    sexpr  : '(' <expr>* ')' ;                         \
    qexpr  : '{' <expr>* '}' ;                         \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ; \
    lispy  : /^/ <expr>* /$/ ;                         \
    ",
    Number, Symbol, SExpression, QExpression, Expression, Lispy);

  lenv* env = lenv_new();

  /* Mathematical Functions */
  lenv_add_builtin(env, "+", builtin_plus);
  lenv_add_builtin(env, "-", builtin_minus);
  lenv_add_builtin(env, "*", builtin_times);
  lenv_add_builtin(env, "/", builtin_divide);

  /* List Functions */
  lenv_add_builtin(env, "list", builtin_list);
  lenv_add_builtin(env, "head", builtin_head);
  lenv_add_builtin(env, "tail", builtin_tail);
  lenv_add_builtin(env, "eval", builtin_eval);
  lenv_add_builtin(env, "join", builtin_join);

  /* functions */
  lenv_add_builtin(env, "def", builtin_def);
  lenv_add_builtin(env, "\\", builtin_lambda);

  define_stdlib(Lispy, env);

  puts("Lispy Version 0.1.0");
  puts("Hit Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    mpc_result_t result;

    if(mpc_parse("<stdin>", input, Lispy, &result)) {
      lval* value = lval_eval(env, lval_read(result.output));
      lval_println(value);
      lval_delete(value);
      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(5, Number, Symbol, SExpression, QExpression, Expression, Lispy);
  lenv_delete(env);

  return 0;
}
