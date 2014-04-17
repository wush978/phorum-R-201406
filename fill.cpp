#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
SEXP fill(NumericVector v, double value) {
  v.fill(value);
  return v;
}
