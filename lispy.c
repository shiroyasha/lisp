#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "lispy.h"

int main(int argc, char **argv) {
  lparser* parser = lparser_new();
  lenv* env = lenv_new();

  define_core_functions(parser, env);

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
