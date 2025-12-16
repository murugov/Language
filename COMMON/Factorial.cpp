#include "Factorial.hpp"


int Factorial(int n)
{
    if (n < 0) return 0;
    if (n <= 1) return 1;
    
    int result = 1;
    for (int i = 2; i <= n; i++)
    {
        if (result > INT_MAX / i) return INT_MAX;
        result *= i;
    }
    return result;
}