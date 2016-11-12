#include "lispy.h"

lparser* lparser_new() {
  mpc_parser_t* Number      = mpc_new("number");
  mpc_parser_t* Symbol      = mpc_new("symbol");
  mpc_parser_t* SExpression = mpc_new("sexpr");
  mpc_parser_t* QExpression = mpc_new("qexpr");
  mpc_parser_t* Expression  = mpc_new("expr");
  mpc_parser_t* Lispy       = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                  \
    number : /-?[0-9]+/ ;                              \
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;        \
    sexpr  : '(' <expr>* ')' ;                         \
    qexpr  : '{' <expr>* '}' ;                         \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ; \
    lispy  : /^/ <expr>* /$/ ;                         \
    ",
    Number, Symbol, SExpression, QExpression, Expression, Lispy);

  lparser* parser = malloc(sizeof(lparser));

  parser->count = 6;
  parser->mpc_parsers = malloc(sizeof(mpc_parser_t*) * 6);
  parser->mpc_parsers[0] = Number;
  parser->mpc_parsers[1] = Symbol;
  parser->mpc_parsers[2] = SExpression;
  parser->mpc_parsers[3] = QExpression;
  parser->mpc_parsers[4] = Expression;
  parser->mpc_parsers[5] = Lispy;

  return parser;
}

void lparser_destroy(lparser* parser) {
  mpc_cleanup(6,
      parser->mpc_parsers[0],
      parser->mpc_parsers[1],
      parser->mpc_parsers[2],
      parser->mpc_parsers[3],
      parser->mpc_parsers[4],
      parser->mpc_parsers[5]);

  free(parser->mpc_parsers);
  free(parser);
}
