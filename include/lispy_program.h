#pragma once

struct lprogram;

typedef struct lprogram lprogram;

struct lprogram {
  lparser* parser;
  lenv* env;
};

lprogram* lprogram_new();
void lprogram_destroy(lprogram* program);
