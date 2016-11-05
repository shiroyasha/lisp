#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

typedef struct {
  int type;
  long number;
  int error;
} lval;

/* lval errors */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* lval types */
enum { LVAL_NUM, LVAL_ERR };

lval lval_number(long number) {
  lval value;
  value.type = LVAL_NUM;
  value.number = number;
  return value;
}

lval lval_error(int error_type) {
  lval error;
  error.type = LVAL_ERR;
  error.error = error_type;
  return error;
}

void lval_print(lval v) {
  switch(v.type) {
    case LVAL_NUM:
      printf("%li", v.number);
      break;

    case LVAL_ERR:
      if (v.error == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if (v.error == LERR_BAD_OP)   {
        printf("Error: Invalid Operator!");
      }
      if (v.error == LERR_BAD_NUM)  {
        printf("Error: Invalid Number!");
      }
      break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

lval lispy_eval_operation(char* operator, lval a, lval b) {
  if (a.type == LVAL_ERR) { return a; }
  if (b.type == LVAL_ERR) { return b; }

  if(strcmp(operator, "+") == 0) { return lval_number(a.number + b.number); }
  if(strcmp(operator, "-") == 0) { return lval_number(a.number - b.number); }
  if(strcmp(operator, "*") == 0) { return lval_number(a.number * b.number); }
  if(strcmp(operator, "/") == 0) {
    return b.number == 0
      ? lval_error(LERR_DIV_ZERO)
      : lval_number(a.number / b.number);
  }

  return lval_error(LERR_BAD_OP);
}

lval lispy_eval(mpc_ast_t *t) {
  if(strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);

    return errno != ERANGE ? lval_number(x) : lval_error(LERR_BAD_NUM);
  }

  char *operator = t->children[1]->contents;

  lval result = lispy_eval(t->children[2]);

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
      lval_println(lispy_eval(result.output));
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
