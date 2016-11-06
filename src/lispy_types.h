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

  /* functions */
  lbuiltin builtin;

  lenv* env;
  lval* formals;
  lval* body;

  /* lists */
  int count;
  struct lval** cell;
};

struct lenv {
  int count;

  struct lenv* parent;

  char** symbols;
  lval** values;
};
