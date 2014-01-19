#ifndef LT_STRINGTOOL_H
#define LT_STRINGTOOL_H

// helper functions for std::string operations (uses c++ name conventions)
// ------------------------------------------------------------------//

#include <string>
#include <vector>
#include <utility>

namespace lt
{
    // does str contains sub_str?
    bool string_contains(const std::string& str, const std::string& sub_str);

    // remove all occurrences of sub_str from str
    std::string string_remove_all(const std::string& str, const std::string& sub_str);

    // replace all occurrences of sub_str from str with new_sub_str
    std::string string_replace_all(const std::string& str, const std::string& sub_str, const std::string& new_sub_str);

    // replace first occurrences of sub_str from str with new_sub_str
    std::string string_replace_first(const std::string& str, const std::string& sub_str, const std::string& new_sub_str);

    // replace last occurrences of sub_str from str with new_sub_str
    std::string string_replace_last(const std::string& str, const std::string& sub_str, const std::string& new_sub_str);

    // replace all upper case characters to lower case 
    std::string string_lower(const std::string& str);

    // replace all lower case characters to upper case 
    std::string string_upper(const std::string& str);

    // convert to double
    double string_to_double(const std::string& str);

    // convert to int
    int string_to_int(const std::string& str);

    // split the string into a vector of string based on the deliminator 
    std::vector<std::string> string_split(const std::string& str, const std::string& delim = ",");

    // join the string into a delim seperated list 
    std::string string_join(const std::vector<std::string>& str_vector, const std::string& delim = ",");

    // character is space
    bool is_space(const char c);

    // character is not a space
    bool not_space(const char c);

    // print the value/error with +/-
    std::string pm(double value, double error, const std::string& precision = "1.2");
    std::string pm(const std::pair<double, double>& value_pair, const std::string& precision = "1.2");
    std::string pm(const std::pair<float, float>& value_pair, const std::string& precision = "1.2");

    // print the value/error with +/- in latex
    std::string pm_latex(double value, double error, const std::string& precision = "1.2");
    std::string pm_latex(const std::pair<double, double>& value_pair, const std::string& precision = "1.2");
    std::string pm_latex(const std::pair<float, float>& value_pair, const std::string& precision = "1.2");

} // namespace lt

#endif // LT_STRINGTOOLS_H
