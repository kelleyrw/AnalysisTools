#ifndef LT_IS_EQUAL_H
#define LT_IS_EQUAL_H

namespace lt
{
    bool is_equal(const int lhs, const int rhs);
    bool is_equal(const long int lhs, const long int rhs);
    bool is_equal(const float lhs, const float rhs);
    bool is_equal(const double lhs, const double rhs);
    bool is_equal(const long double lhs, const long double rhs);
    bool is_equal(const float lhs, const float rhs, const float relative_tolerance, const float absolute_tolerance);
    bool is_equal(const double lhs, const double rhs, const double relative_tolerance, const double absolute_tolerance);
    bool is_equal(const long double lhs, const long double rhs, const long double relative_tolerance, const long double absolute_tolerance);

} // namespace lt

#endif // LT_IS_EQUAL_H
