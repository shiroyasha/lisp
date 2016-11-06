#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->parent  = NULL;
  e->count   = 0;
  e->symbols = NULL;
  e->values  = NULL;
  return e;
}

void lenv_delete(lenv* e) {
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
    if(strcmp(env->symbols[i], key->symbol) == 0) {
      return lval_copy(env->values[i]);
    }
  }

  if(env->parent) {
    return lenv_get(env->parent, key);
  } else {
    return lval_error("unbound symbol");
  }
}

void lenv_put(lenv* env, lval* key, lval* value) {
  for(int i=0; i < env->count; i++) {
    if(strcmp(env->symbols[i], key->symbol) == 0) {
      lval_delete(env->values[i]);

      env->values[i] = lval_copy(value);
      return;
    }
  }

  env->count++;
  env->symbols = realloc(env->symbols, sizeof(char*) * env->count);
  env->values  = realloc(env->values, sizeof(lval*) * env->count);

  env->symbols[env->count - 1] = malloc(strlen(key->symbol) + 1);
  env->values[env->count - 1] = lval_copy(value);

  strcpy(env->symbols[env->count - 1], key->symbol);
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin builtin) {
  lval* k = lval_symbol(name);
  lval* v = lval_builtin(builtin);
  lenv_put(e, k, v);
  lval_delete(k);
  lval_delete(v);
}

lenv* lenv_copy(lenv* e) {
  lenv* n = malloc(sizeof(lenv));

  n->parent  = e->parent;
  n->count   = e->count;
  n->symbols = malloc(sizeof(char*) * n->count);
  n->values  = malloc(sizeof(lval*) * n->count);

  for (int i = 0; i < e->count; i++) {
    n->symbols[i] = malloc(strlen(e->symbols[i]) + 1);
    strcpy(n->symbols[i], e->symbols[i]);

    n->values[i] = lval_copy(e->values[i]);
  }

  return n;
}

void lenv_print(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    printf("%s: ", e->symbols[i]);
    lval_println(e->values[i]);
  }

  if(e->parent) {
    lenv_print(e->parent);
  }
}
