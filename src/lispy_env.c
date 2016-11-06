#include <stdlib.h>
#include <stdio.h>

#include "lispy.h"

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->symbols = NULL;
  e->values = NULL;
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

  return lval_error("unbound symbol");
}

void lenv_put(lenv* env, lval* key, lval* value) {
  for(int i=0; i < env->count; i++) {
    if(strcmp(env->symbols[i], key->symbol) == 0) {
      printf("inside\n");
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
