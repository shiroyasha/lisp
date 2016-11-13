#include "lispy.h"

void define_core_functions(lparser* parser, lenv* env) {
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

  lenv_add_builtin(env, "do", builtin_do);

  lispy_eval(parser, env, "def {fun} (\\ {args body} {def (list (head args)) (\\ (tail args) body)})");
}
