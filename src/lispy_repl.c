#include "lispy.h"

void repl_start(lparser* parser, lenv* env) {
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
}
