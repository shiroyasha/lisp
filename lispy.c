#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

long lispy_eval_operation(char* operator, long a, long b) {
  if(strcmp(operator, "+") == 0) { return a + b; }
  if(strcmp(operator, "-") == 0) { return a - b; }
  if(strcmp(operator, "*") == 0) { return a * b; }
  if(strcmp(operator, "/") == 0) { return a / b; }
  return 0;
}

long lispy_eval(mpc_ast_t *t) {
  if(strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  char *operator = t->children[1]->contents;

  long result = lispy_eval(t->children[2]);

  for(int i=3; i < t->children_num; i++) {
    if(strstr(t->children[i]->tag, "expression")) {
      result = lispy_eval_operation(operator, result, lispy_eval(t->children[i]));
    }
  }

  return result;
}

int main(int argc, char **argv) {
  mpc_parser_t* Number     = mpc_new("number");
  mpc_parser_t* Operator   = mpc_new("operator");
  mpc_parser_t* Expression = mpc_new("expression");
  mpc_parser_t* Lispy      = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                          \
    number     : /-?[0-9]+/ ;                                  \
    operator   : '+' | '-' | '*' | '/' ;                       \
    expression : <number> | '(' <operator> <expression>+ ')' ; \
    lispy      : /^/ <operator> <expression>+ /$/ ;            \
    ",
    Number, Operator, Expression, Lispy);

  puts("Lispy Version 0.0.2");
  puts("Hit Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    mpc_result_t result;

    if(mpc_parse("<stdin>", input, Lispy, &result)) {
      printf("%ld\n", lispy_eval(result.output));
      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expression, Lispy);

  return 0;
}
