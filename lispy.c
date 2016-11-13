#include "lispy.h"

int main(int argc, char **argv) {
  lparser* parser = lparser_new();
  lenv* env = lenv_new();

  define_core_functions(parser, env);

  repl_start(parser, env);

  lenv_delete(env);
  lparser_destroy(parser);

  return 0;
}
