#ifndef RT_MISCTOOLS_H
#define RT_MISCTOOLS_H

// miscellaneous helper and functions for ROOT related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// c++ includes
#include <string>
#include <map>
#include <vector>

// ROOT includes
#include "TChain.h"

// namespace rt --> root tools
namespace rt
{
    // print a single object to eps/png/pdf
    template <typename RootObjectType>
    void Print
    (
         RootObjectType obj, 
         const std::string& file_name, 
         const std::string& suffix = "png", 
         const std::string& option = "",
         bool logy = false
    );

    // print the plots from the map to eps/png/pdf
    template <typename RootObjectType>
    void Print
    (
         std::map<std::string, RootObjectType>& m, 
         const std::string& dir_name, 
         const std::string& suffix = "png", 
         const std::string& option = "",
         const bool logy = false
    );

    // make a TChain from a path
    TChain* MakeTChain
    (
        const std::string& glob, 
        const std::string& treename = "Events", 
        TChain* const chain_in = NULL, 
        const bool verbose = false
    );

    // create a chain from a comma serperated list (e.g. "file1,file2,...")
    TChain* CreateTChainFromCommaSeperatedList
    (
        const std::string& str, 
        const std::string& treename,
        const std::string& prefix = "" 
    );

    // create a chain from a vector of file name
    TChain* CreateTChain
    (
        const std::string& treename,
        const std::vector<std::string>& str_vec 
    );

    // print list of files in a TChain
    void PrintFilesFromTChain(TChain * const chain);
    void PrintFilesFromTChain(const TChain& chain);
    void PrintFilesFromTChain(const std::vector<TChain*>& chains);

    // print list of files in a TChain
    std::vector<std::string> GetFilesFromTChain(TChain* const chain);

    // add root files (returns 0 if successful) -- lower case to match the utility name
    int hadd(const std::string& target, const std::vector<std::string>& sources);

    // Add with error
    std::pair<double, double> AddWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2);
    std::pair<float, float>   AddWithError(const std::pair<float, float>& v1  , const std::pair<float, float>& v2  );

    // Subtract with error
    std::pair<double, double> SubtractWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2);
    std::pair<float, float>   SubtractWithError(const std::pair<float, float>& v1  , const std::pair<float, float>& v2  );

    // Multiply with error
    std::pair<double, double> MultiplyWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2);
    std::pair<float, float>   MultiplyWithError(const std::pair<float, float>& v1  , const std::pair<float, float>& v2  );

    // Divide with error
    std::pair<double, double> DivideWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2);
    std::pair<float, float>   DivideWithError(const std::pair<float, float>& v1  , const std::pair<float, float>& v2  );

    // Set style
    void SetStyle(const std::string& value = "emrou");

    // copy the index.php file to dirname
    void CopyIndexPhp(const std::string& target_dir);

} // namespace rt 

// definitions of templated functions
#include "AnalysisTools/RootTools/src/MiscTools.impl.h"

#endif // RT_MISCTOOLS_H
