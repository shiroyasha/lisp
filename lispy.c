#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "lispy.h"

int main(int argc, char **argv) {
  lparser* parser = lparser_new();
  lenv* env = lenv_new();

  /* Mathematical Functions */
  lenv_add_builtin(env, "+", builtin_plus);
  lenv_add_builtin(env, "-", builtin_minus);
  lenv_add_builtin(env, "*", builtin_times);
  lenv_add_builtin(env, "/", builtin_divide);

  /* List Functions */
  lenv_add_builtin(env, "list", builtin_list);
  lenv_add_builtin(env, "head", builtin_head);
  lenv_add_builtin(env, "tail", builtin_tail);
  lenv_add_builtin(env, "eval", builtin_eval);
  lenv_add_builtin(env, "join", builtin_join);

  /* functions */
  lenv_add_builtin(env, "def", builtin_def);
  lenv_add_builtin(env, "\\", builtin_lambda);

  lispy_eval(parser, env, "def {fun} (\\ {args body} {def (list (head args)) (\\ (tail args) body)})");

  puts("Lispy Version 0.1.0");
  puts("Hit Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    lval* value = lispy_eval(parser, env, input);
    lval_println(value);
    lval_delete(value);
    free(input);
  }

  lenv_delete(env);
  lparser_destroy(parser);

  return 0;
}
