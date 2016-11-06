#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

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
