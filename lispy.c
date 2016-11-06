#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

typedef struct lval {
  int type;
  long number;

  char* error;
  char* symbol;

  int count;
  struct lval** cell;
} lval;


/*
 * lval constructors
 */

enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

lval* lval_number(long number) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_NUM;
  value->number = number;
  return value;
}

lval* lval_error(char* error_description) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_ERR;
  value->error = malloc(strlen(error_description) + 1);
  strcpy(value->error, error_description);
  return value;
}

lval* lval_symbol(char* symbol) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_SYM;
  value->symbol = malloc(strlen(symbol) + 1);
  strcpy(value->symbol, symbol);
  return value;
}

lval* lval_sexpr(void) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_SEXPR;
  value->count = 0;
  value->cell = NULL;
  return value;
}

/*
 * lval destructor
 */

void lval_delete(lval* value) {
  switch(value->type) {
    case LVAL_NUM:
      break;

    case LVAL_ERR:
      free(value->error);
      break;

    case LVAL_SYM:
      free(value->symbol);
      break;

    case LVAL_SEXPR:
      for(int i=0; i < value->count; i++) {
        lval_delete(value->cell[i]);
      }

      free(value->cell);
      break;
  }

  free(value);
}

/*
 * printing
 */

void lval_print(lval* value);

void lval_expr_print(lval* value, char open, char close) {
  putchar(open);

  for(int i=0; i < value->count; i++) {
    lval_print(value->cell[i]);

    if(i != value->count - 1) {
      putchar(' ');
    }
  }

  putchar(close);
}

void lval_print(lval* value) {
  switch(value->type) {
    case LVAL_NUM: printf("%li", value->number); break;
    case LVAL_ERR: printf("ERROR: %s",  value->error); break;
    case LVAL_SYM: printf("%s",  value->symbol); break;
    case LVAL_SEXPR: lval_expr_print(value, '(', ')'); break;
  }
}

void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}

/*
 * evaluation
 */

lval* lval_pop(lval* value, int index) {
  lval* x = value->cell[index];

  memmove(&value->cell[index], &value->cell[index+1], sizeof(lval*) * (value->count - index - 1));

  value->count--;

  value->cell = realloc(value->cell, sizeof(lval*) * value->count);

  return x;
}

lval* lval_take(lval* value, int index) {
  lval* x = lval_pop(value, index);
  lval_delete(value);
  return x;
}

lval* builtin_operation_plus(lval* numbers) {
  long result = 0;

  for(int i=0; i < numbers->count; i++) {
    result += numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_operation_minus(lval* numbers) {
  if(numbers->count == 1) {
    return lval_number(-numbers->cell[0]->number);
  }

  long result = numbers->cell[0]->number;

  for(int i=1; i < numbers->count; i++) {
    result -= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_operation_times(lval* numbers) {
  long result = 1;

  for(int i=0; i < numbers->count; i++) {
    result *= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_operation_divide(lval* numbers) {
  long result = numbers->cell[0]->number;

  for(int i=1; i < numbers->count; i++) {
    if(numbers->cell[i]->number == 0) {
      return lval_error("Division By Zero!");
    }

    result /= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_operation(char* operator, lval* numbers) {
  for(int i=0; i < numbers->count; i++) {
    if(numbers->cell[i]->type != LVAL_NUM) {
      return lval_error("Cannot operate on non-number!");
    }
  }

  if(strcmp(operator, "+") == 0) { return builtin_operation_plus(numbers); }
  if(strcmp(operator, "-") == 0) { return builtin_operation_minus(numbers); }
  if(strcmp(operator, "*") == 0) { return builtin_operation_times(numbers); }
  if(strcmp(operator, "/") == 0) { return builtin_operation_divide(numbers); }

  return lval_error("Unrecognized operator");
}

lval* lval_eval(lval* value);

lval* lval_eval_sexpr(lval* value) {
  /* evaluate every child */
  for(int i=0; i < value->count; i++) {
    value->cell[i] = lval_eval(value->cell[i]);
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
  if(f->type != LVAL_SYM) {
    lval_delete(value);
    lval_delete(f);
    return lval_error("S-expression Does not start with symbol!");
  }

  lval* result = builtin_operation(f->symbol, value);
  lval_delete(f);
  lval_delete(value);
  return result;
}

lval* lval_eval(lval* value) {
  if(value->type == LVAL_SEXPR) { return lval_eval_sexpr(value); }

  return value;
}

/*
 * reader
 */

lval* lval_read_number(mpc_ast_t *t) {
  errno = 0;

  long number = strtol(t->contents, NULL, 10);

  return errno != ERANGE ? lval_number(number) : lval_error("invalid number");
}

lval* lval_add(lval* a, lval* b) {
  a->count++;
  a->cell = realloc(a->cell, sizeof(lval*) * a->count);
  a->cell[a->count-1] = b;
  return a;
}

lval* lval_read(mpc_ast_t *t) {
  if(strstr(t->tag, "number")) { return lval_read_number(t); }
  if(strstr(t->tag, "symbol")) { return lval_symbol(t->contents); }

  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }

    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}

int main(int argc, char **argv) {
  mpc_parser_t* Number      = mpc_new("number");
  mpc_parser_t* Symbol      = mpc_new("symbol");
  mpc_parser_t* SExpression = mpc_new("sexpression");
  mpc_parser_t* Expression  = mpc_new("expression");
  mpc_parser_t* Lispy       = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                   \
    number      : /-?[0-9]+/ ;                          \
    symbol      : '+' | '-' | '*' | '/' ;               \
    sexpression : '(' <expression>* ')' ;               \
    expression  : <number> | <symbol> | <sexpression> ; \
    lispy       : /^/ <expression>* /$/ ;    \
    ",
    Number, Symbol, SExpression, Expression, Lispy);

  puts("Lispy Version 0.1.0");
  puts("Hit Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    mpc_result_t result;

    if(mpc_parse("<stdin>", input, Lispy, &result)) {
      lval* value = lval_eval(lval_read(result.output));
      lval_println(value);
      lval_delete(value);
      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(5, Number, Symbol, SExpression, Expression, Lispy);

  return 0;
}
