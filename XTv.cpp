#include <Rcpp.h>
using namespace Rcpp;
// [[Rcpp::export]]
SEXP XTv(S4 m, NumericVector v, NumericVector& retval) {
  int
  *i = INTEGER(wrap(m.slot("i"))),
  i_len = Rf_length(wrap(m.slot("i"))),
  *p = INTEGER(wrap(m.slot("p"))),
  p_len = Rf_length(wrap(m.slot("p")));
  NumericVector x = wrap(m.slot("x"));
  IntegerVector dim(wrap(m.slot("Dim")));
  if (v.size() != dim[0]) throw std::invalid_argument("");
  for(int j = 0,k = 0;j < p_len - 1;j++) {
    while(k < p[j+1]) {
      retval[j] += x[k]*v[i[k++]];
    }
  }
  return retval;
}
