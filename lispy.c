#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

struct lval;
struct lenv;

enum { LVAL_NUM, LVAL_ERR, LVAL_FUN, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;
  long number;

  char* error;
  char* symbol;

  lbuiltin fun;

  int count;
  struct lval** cell;
};

struct lenv {
  int count;

  char** symbols;
  lval** values;
};


/*
 * lval constructors
 */

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

lval* lval_qexpr(void) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_QEXPR;
  value->count = 0;
  value->cell = NULL;
  return value;
}

lval* lval_fun(lbuiltin fun) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_FUN;
  value->fun  = fun;
  return value;
}


/*
 * lval destructor
 */

void lval_delete(lval* value) {
  switch(value->type) {
    case LVAL_FUN:
    case LVAL_NUM:
      break;

    case LVAL_ERR:
      free(value->error);
      break;

    case LVAL_SYM:
      free(value->symbol);
      break;

    case LVAL_SEXPR:
    case LVAL_QEXPR:
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
    case LVAL_FUN:   printf("<function>");               break;
    case LVAL_NUM:   printf("%li", value->number);       break;
    case LVAL_ERR:   printf("ERROR: %s",  value->error); break;
    case LVAL_SYM:   printf("%s",  value->symbol);       break;
    case LVAL_SEXPR: lval_expr_print(value, '(', ')');   break;
    case LVAL_QEXPR: lval_expr_print(value, '{', '}');   break;
  }
}

void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}

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

lval* lval_add(lval* a, lval* b) {
  a->count++;
  a->cell = realloc(a->cell, sizeof(lval*) * a->count);
  a->cell[a->count-1] = b;
  return a;
}

lval* lval_join(lval* a, lval* b) {
  while(b->count) {
    a = lval_add(a, lval_pop(b, 0));
  }

  lval_delete(b);
  return a;
}

lval* lval_copy(lval* a) {
  lval* b = malloc(sizeof(lval*));
  b->type = a->type;

  switch(a->type) {
    case LVAL_FUN:
      b->fun = a->fun;
      break;

    case LVAL_NUM:
      b->number = a->number;
      break;

    case LVAL_ERR:
      b->error = malloc(strlen(a->error) + 1);
      strcpy(b->error, a->error);
      break;

    case LVAL_SYM:
      b->symbol = malloc(strlen(a->symbol) + 1);
      strcpy(b->symbol, a->symbol);
      break;

    case LVAL_SEXPR:
    case LVAL_QEXPR:
      b->count = a->count;
      b->cell = malloc(sizeof(lval*) * a->count);

      for(int i=0; i < a->count; i++) {
        b->cell[i] = lval_copy(a->cell[i]);
      }

      break;
  }

  return b;
}

/*
 * lenv
 */

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->symbols = NULL;
  e->values = NULL;
  return e;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->symbols[i]);
    lval_delete(e->values[i]);
  }

  free(e->symbols);
  free(e->values);
  free(e);
}

lval* lenv_get(lenv* env, lval* key) {
  for(int i=0; i < env->count; i++) {
    if(strcmp(env->symbols[i], key->symbol)) {
      return lval_copy(env->values[i]);
    }
  }

  return lval_error("unbound symbol");
}

lenv* lenv_put(lenv* env, lval* key, lval* value) {
  for(int i=0; i < env->count; i++) {
    if(strcmp(env->symbols[i], key->symbol)) {
      lval_delete(env->values[i]);

      env->values[i] = lval_copy(value);
    }
  }

  env->count++;
  env->symbols = realloc(env->symbols, sizeof(lval*) * env->count);
  env->values  = realloc(env->values, sizeof(char*) * env->count);

  env->symbols[env->count - 1] = malloc(strlen(key->symbol) + 1);
  env->values[env->count - 1] = lval_copy(value);

  strcpy(env->symbols[env->count - 1], key->symbol);

  return env;
}

/*
 * builtin functions
 */

lval* builtin_plus(lval* numbers) {
  long result = 0;

  for(int i=0; i < numbers->count; i++) {
    if(numbers->cell[i]->type != LVAL_NUM) {
      return lval_error("+ cannot operate on non-number!");
    }

    result += numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_minus(lval* numbers) {
  if(numbers->cell[0]->type != LVAL_NUM) {
    return lval_error("- cannot operate on non-number!");
  }

  if(numbers->count == 1) {
    return lval_number(-numbers->cell[0]->number);
  }

  long result = numbers->cell[0]->number;

  for(int i=1; i < numbers->count; i++) {
    if(numbers->cell[i]->type != LVAL_NUM) {
      return lval_error("- cannot operate on non-number!");
    }

    result -= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_times(lval* numbers) {
  long result = 1;

  for(int i=0; i < numbers->count; i++) {
    if(numbers->cell[i]->type != LVAL_NUM) {
      return lval_error("* cannot operate on non-number!");
    }

    result *= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_divide(lval* numbers) {
  long result = numbers->cell[0]->number;

  for(int i=1; i < numbers->count; i++) {
    if(numbers->cell[i]->type != LVAL_NUM) {
      return lval_error("/ cannot operate on non-number!");
    }

    if(numbers->cell[i]->number == 0) {
      return lval_error("Division By Zero!");
    }

    result /= numbers->cell[i]->number;
  }

  return lval_number(result);
}

lval* builtin_list(lval* value) {
  lval* result = lval_qexpr();
  result->count = value->count;
  result->cell = value->cell;

  value->count = 0;

  return result;
}

lval* builtin_head(lval* value) {
  if(value->count != 1)                  { return lval_error("Function head expects one argument"); }
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function head called with incorect datatype"); }
  if(value->cell[0]->count == 0)         { return lval_error("Function head called on empty list"); }

  return lval_pop(value->cell[0], 0);
}

lval* builtin_tail(lval* value) {
  if(value->count != 1)                  { return lval_error("Function tail expects one argument"); }
  if(value->cell[0]->type != LVAL_QEXPR) { return lval_error("Function tail called with incorect datatype"); }
  if(value->cell[0]->count == 0)         { return lval_error("Function tail called on empty list"); }

  lval* first_argument = lval_pop(value, 0);
  lval* first_element = lval_pop(first_argument, 0);

  lval_delete(first_element);

  return first_argument;
}

lval* builtin_join(lval* value) {
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

lval* lval_eval(lval* value);

lval* builtin_eval(lval* value) {
  if(value->count != 1)         { return lval_error("Function eval expects one argument"); }
  if(value->type == LVAL_QEXPR) { return lval_error("Function eval called with incorect datatype"); }

  lval* result = lval_pop(value, 0);
  result->type = LVAL_SEXPR;

  return lval_eval(result);
}

lval* builtin(char* operator, lval* elements) {
  if(strcmp(operator, "+") == 0)    { return builtin_plus(elements);   }
  if(strcmp(operator, "-") == 0)    { return builtin_minus(elements);  }
  if(strcmp(operator, "*") == 0)    { return builtin_times(elements);  }
  if(strcmp(operator, "/") == 0)    { return builtin_divide(elements); }
  if(strcmp(operator, "list") == 0) { return builtin_list(elements);   }
  if(strcmp(operator, "head") == 0) { return builtin_head(elements);   }
  if(strcmp(operator, "tail") == 0) { return builtin_tail(elements);   }
  if(strcmp(operator, "join") == 0) { return builtin_join(elements);   }
  if(strcmp(operator, "eval") == 0) { return builtin_eval(elements);   }

  return lval_error("Unrecognized operator");
}

/*
 * evaluation
 */

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

  lval* result = builtin(f->symbol, value);
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

lval* lval_read(mpc_ast_t *t) {
  if(strstr(t->tag, "number")) { return lval_read_number(t); }
  if(strstr(t->tag, "symbol")) { return lval_symbol(t->contents); }

  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }

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
  mpc_parser_t* SExpression = mpc_new("sexpr");
  mpc_parser_t* QExpression = mpc_new("qexpr");
  mpc_parser_t* Expression  = mpc_new("expr");
  mpc_parser_t* Lispy       = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                  \
    number : /-?[0-9]+/ ;                              \
    symbol : \"list\"                                  \
           | \"head\"                                  \
           | \"tail\"                                  \
           | \"join\"                                  \
           | \"eval\"                                  \
           | '+'                                       \
           | '-'                                       \
           | '*'                                       \
           | '/' ;                                     \
    sexpr  : '(' <expr>* ')' ;                         \
    qexpr  : '{' <expr>* '}' ;                         \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ; \
    lispy  : /^/ <expr>* /$/ ;                         \
    ",
    Number, Symbol, SExpression, QExpression, Expression, Lispy);

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

  mpc_cleanup(5, Number, Symbol, SExpression, QExpression, Expression, Lispy);

  return 0;
}
