#include "math_func.hpp"
#include "is_zero.hpp"


double acot(double x)
{
    if (is_zero(x))
        return M_PI / 2.0;
    else if (x > 0.0)
        return atan(1.0 / x);
    else
        return atan(1.0 / x) + M_PI;
}


double acoth(double x)
{
    if (is_zero(x - 1))
        return NAN;
    
    return 0.5 * log((x + 1.0) / (x - 1.0));
}