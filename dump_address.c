#define USE_RINTERNALS
#include <R.h>
#include <Rdefines.h>

SEXP dump_address(SEXP src) {
  Rprintf("%16p %16p %d\n", &(src->u), INTEGER(src), INTEGER(src) - (int*)&(src->u));
  return R_NilValue;
}