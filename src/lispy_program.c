#include "lispy.h"

lprogram* lprogram_new() {
  lprogram* program = malloc(sizeof(lprogram));

  program->parser = lparser_new();
  program->env    = lenv_new();

  define_core_functions(program->parser, program->env);

  return program;
}

void lprogram_destroy(lprogram* program) {
  lenv_delete(program->env);
  lparser_destroy(program->parser);

  free(program);
}
