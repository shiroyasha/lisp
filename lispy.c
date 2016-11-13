#include "lispy.h"

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

void run_file(lprogram* program, char* filename) {
  int fd = open(filename, O_RDONLY);
  int len = lseek(fd, 0, SEEK_END);

  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

  lval* result = lispy_eval(program->parser, program->env, (char*)data);

  lval_println(result);
  lval_delete(result);
}

int main(int argc, char **argv) {
  lprogram* program = lprogram_new();

  switch(argc) {
    case 1: /* no arguments */
      repl_start(program);
      break;

    case 2: /* given a filename */
      run_file(program, argv[1]);
      break;

    default:
      puts("Error, unrecognized parameters");
      return 1;
  }

  lprogram_destroy(program);

  return 0;
}
