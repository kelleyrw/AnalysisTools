#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

int main()
{
    const std::vector<std::string> str_vec  = {"genps_1", "genps_1", "genps_2", "blah_1", "blah_2"};
    const std::vector<std::string> patterns = {"genps(.*)"};
    const std::vector<std::regex> exps(patterns.begin(), patterns.end());
    std::vector<std::string> matched_str_vec;
    for (const auto& str : str_vec)
    {
        for (const auto& exp : exps) 
        {
            if (std::regex_match(str, exp)) matched_str_vec.push_back(str);
        }
    }
    std::cout << "before match = " << lt::ArrayString(str_vec)         << "\n";
    std::cout << "patterns     = " << lt::ArrayString(patterns)        << "\n";
    std::cout << "after match  = " << lt::ArrayString(matched_str_vec) << "\n";
}
