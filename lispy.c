#include "lispy.h"

int main(int argc, char **argv) {
  lprogram* program = lprogram_new();

  repl_start(program);

  lprogram_destroy(program);

  return 0;
}
