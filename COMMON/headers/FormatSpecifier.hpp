#ifndef FORMAT_SPECIFIER_HPP
#define FORMAT_SPECIFIER_HPP

#define FORMAT_SPECIFIER(value) _Generic((value), \
    int: "%d", \
    long: "%ld", \
    unsigned int: "%u", \
    unsigned long: "%zu", \
    float: "%.6f", \
    double: "%.6f", \
    default: "%p")

#endif