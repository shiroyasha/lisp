#include "lispy.h"

void repl_start(lprogram* program) {
  puts("Lispy Version 0.1.0");
  puts("Hit Ctrl+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);

    lval* value = lispy_eval(program->parser, program->env, input);
    lval_println(value);
    lval_delete(value);
    free(input);
  }
}
