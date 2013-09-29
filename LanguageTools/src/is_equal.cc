#include "AnalysisTools/LanguageTools/interface/is_equal.h"
#include <cstdlib>
#include <cmath>

namespace lt
{
    bool is_equal(const int lhs, const int rhs)
    {
        return lhs == rhs;
    }

    bool is_equal(const long int lhs, const long int rhs)
    {
        return lhs == rhs;
    }

    bool is_equal(const float lhs, const float rhs)
    {
        const float relative_tolerance = 0.00001f;
        const float absolute_tolerance = 0.000001f;
        return is_equal(lhs, rhs, relative_tolerance, absolute_tolerance);
    }

    bool is_equal(const double lhs, const double rhs)
    {
        const double relative_tolerance = 0.00001;
        const double absolute_tolerance = 0.000001;
        return is_equal(lhs, rhs, relative_tolerance, absolute_tolerance);
    }

    bool is_equal(const long double lhs, const long double rhs)
    {
        const long double relative_tolerance = 0.00001L;
        const long double absolute_tolerance = 0.000001L;
        return is_equal(lhs, rhs, relative_tolerance, absolute_tolerance);
    }

    bool is_equal(const float lhs, const float rhs, const float relative_tolerance, const float absolute_tolerance)
    {
        return fabs(rhs - lhs) <= (relative_tolerance * (fabs(lhs) + fabs(rhs)) + absolute_tolerance);
    }

    bool is_equal(const double lhs, const double rhs, const double relative_tolerance, const double absolute_tolerance)
    {
        return fabs(rhs - lhs) <= (relative_tolerance * (fabs(lhs) + fabs(rhs)) + absolute_tolerance);
    }

    bool is_equal(const long double lhs, const long double rhs, const long double relative_tolerance, const long double absolute_tolerance)
    {
        return fabs(rhs - lhs) <= (relative_tolerance * (fabs(lhs) + fabs(rhs)) + absolute_tolerance);
    }

} // namespace lt
