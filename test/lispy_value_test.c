#include <assert.h>
#include <stdio.h>

#include "lispy.h"

void test_creating_error_value() {
  printf(" - test_creating_error_value\n");

  lval *lval = lval_error("some error with numbers %d", 12);

  assert(lval->type == LVAL_ERR);
  assert(strcmp(lval->error, "some error with numbers 12") == 0);

  lval_delete(lval);
}

void test_creating_symbol_value() {
  printf(" - test_creating_symbol_value\n");

  lval *lval = lval_symbol("test");

  assert(lval->type == LVAL_SYM);
  assert(strcmp(lval->symbol, "test") == 0);

  lval_delete(lval);
}

void test_creating_number_value() {
  printf(" - test_creating_number_value\n");

  lval *lval = lval_number(23);

  assert(lval->type == LVAL_NUM);
  assert(lval->number == 23);

  lval_delete(lval);
}

int main() {
  test_creating_error_value();
  test_creating_symbol_value();
  test_creating_number_value();
}
