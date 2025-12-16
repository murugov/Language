#include "is_zero.hpp"

int is_zero(double x) { return (fabs(x) < 1e-10); }