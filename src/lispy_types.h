#pragma once

#include "lispy.h"

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

  lbuiltin builtin;

  int count;
  struct lval** cell;
};

struct lenv {
  int count;

  char** symbols;
  lval** values;
};
