#pragma once

struct lparser;

typedef struct lparser lparser;

struct lparser {
  int count;
  mpc_parser_t** mpc_parsers;
};

lparser* lparser_new();
void lparser_destroy(lparser* parser);

/* lparser* lparser_parse(lparser* parser, char* input); */
