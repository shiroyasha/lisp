#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lval* lval_number(long number) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_NUM;
  value->number = number;
  return value;
}

lval* lval_error(char* format, ...) {
  lval* value = malloc(sizeof(lval));
  value->type = LVAL_ERR;

  va_list va;
  va_start(va, format);

  /* set maximux error description size to 512 bytes */
  value->error = malloc(512);

  /* populate the error description */
  vsnprintf(value->error, 511, format, va);

  /* release unnused memory */
  value->error = realloc(value->error, strlen(value->error) + 1);

  va_end(va);

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

lval* lval_builtin(lbuiltin builtin) {
  lval* value    = malloc(sizeof(lval));
  value->type    = LVAL_FUN;
  value->builtin = builtin;

  return value;
}

lval* lval_lambda(lval* formals, lval* body) {
  lval* value    = malloc(sizeof(lval));
  value->type    = LVAL_FUN;
  value->builtin = NULL;

  /* Build new environment */
  value->env = lenv_new();

  /* Set Formals and Body */
  value->formals = formals;
  value->body = body;

  return value;
}

void lval_delete(lval* value) {
  switch(value->type) {
    case LVAL_FUN:
      if (!value->builtin) {
        lenv_delete(value->env);
        lval_delete(value->formals);
        lval_delete(value->body);
      }
      break;

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

void lval_print_fun(lval* value) {
  if(value->builtin) {
    printf("<function>");
  } else {
    printf("(\\ ");
    lval_print(value->formals);
    printf(" ");
    lval_print(value->body);
    printf(")");
  }
}

void lval_print(lval* value) {
  switch(value->type) {
    case LVAL_FUN:   lval_print_fun(value);              break;
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
  lval* b = malloc(sizeof(lval));
  b->type = a->type;

  switch(a->type) {
    case LVAL_FUN:
      if(a->builtin) {
        b->builtin = a->builtin;
      } else {
        b->builtin = NULL;
        b->env     = lenv_copy(a->env);
        b->formals = lval_copy(a->formals);
        b->body    = lval_copy(a->body);
      }
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
