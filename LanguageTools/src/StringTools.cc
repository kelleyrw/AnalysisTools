#include "AnalysisTools/LanguageTools/interface/StringTools.h"

// helper functions for std::string operations (uses c++ name conventions)
// ------------------------------------------------------------------//

// boost includes
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

// ROOT includes
#include "TString.h"

namespace lt
{
    // does str contain sub_str?
    bool string_contains(const std::string& str, const std::string& sub_str)
    {
        return std::string::npos != str.find(sub_str);
    }

    // split the string into a vector of string based on the deliminator 
    std::vector<std::string> string_split(const std::string& str, const std::string& delim)
    {
        std::vector<std::string> str_vector;
        boost::split(str_vector, str, boost::is_any_of(delim));
        return str_vector; 
    }

    // join the string into a delim seperated list 
    std::string string_join(const std::vector<std::string>& str_vector, const std::string& delim)
    {
        std::string result = boost::join(str_vector, delim);
        return result; 
    }

    // remove all occurrences of sub_str from str
    std::string string_remove_all(const std::string& str, const std::string& sub_str)
    {
        std::string result(str);
        while(string_contains(result, sub_str))
        {
            size_t pos = result.find(sub_str);
            result.erase(pos, sub_str.size());
        }
        return result;
    }

    // replace all occurrences of sub_str from str with new_sub_str
    std::string string_replace_all(const std::string& str, const std::string& sub_str, const std::string& new_sub_str)
    {
        std::string result(str);
        boost::replace_all(result, sub_str, new_sub_str); 
        return result;
    }

    // replace first occurrence of sub_str from str with new_sub_str
    std::string string_replace_first(const std::string& str, const std::string& sub_str, const std::string& new_sub_str)
    {
        std::string result(str);
        boost::replace_first(result, sub_str, new_sub_str); 
        return result;
    }

    // replace last occurrence of sub_str from str with new_sub_str
    std::string string_replace_last(const std::string& str, const std::string& sub_str, const std::string& new_sub_str)
    {
        std::string result(str);
        boost::replace_first(result, sub_str, new_sub_str); 
        return result;
    }

    // replace all upper case characters to lower case 
    std::string string_lower(const std::string& str)
    {
        std::string result(str);
        boost::to_lower(result);
        return result;
    }

    // replace all lower case characters to upper case 
    std::string string_upper(const std::string& str)
    {
        std::string result(str);
        boost::to_upper(result);
        return result;
    }

    // convert to double
    double string_to_double(const std::string& str)
    {
        return boost::lexical_cast<double>(str);
    }

    // convert to int
    int string_to_int(const std::string& str)
    {
        return boost::lexical_cast<int>(str);
    }

    // character is space
    bool is_space(char c)
    {
        return static_cast<bool>(isspace(c));
    }

    // character is not a space
    bool not_space(char c)
    {
        return !is_space(c);
    }

    // print value and error with +/-
    std::string pm(double value, double error, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f +/- %" + precision +"f";
        return std::string(Form(format_string.c_str(), value, error));
    }

    std::string pm(const std::pair<double, double>& value_pair, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f +/- %" + precision +"f";
        return std::string(Form(format_string.c_str(), value_pair.first, value_pair.second));
    }
    
    std::string pm(const std::pair<float, float>& value_pair, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f +/- %" + precision +"f";
        return std::string(Form(format_string.c_str(), value_pair.first, value_pair.second));
    }

    // print value and error with +/- in latex
    std::string pm_latex(double value, double error, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f $\\pm$ %" + precision +"f";
        return std::string(Form(format_string.c_str(), value, error));
    }

    std::string pm_latex(const std::pair<double, double>& value_pair, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f $\\pm$ %" + precision +"f";
        return std::string(Form(format_string.c_str(), value_pair.first, value_pair.second));
    }
    
    std::string pm_latex(const std::pair<float, float>& value_pair, const std::string& precision)
    {
        std::string format_string = "%" + precision + "f $\\pm$ %" + precision +"f";
        return std::string(Form(format_string.c_str(), value_pair.first, value_pair.second));
    }

} // namespace lt
