// c++
#include <iostream>
#include <string>

// ROOT
#include "TChain.h"
#include "AnalysisTools/RootTools/interface/MiscTools.h"
#include "AnalysisTools/LanguageTools/interface/OSTools.h"

// BOOST
#include <boost/program_options.hpp>

// merged a TChain
void MergedTChain(const std::string& input_path, const std::string& merged_file, const std::string& tree_name, const std::string& option)
{
    std::cout << Form("[merge_tchain] merging %s to %s", input_path.c_str(), merged_file.c_str()) << std::endl;
    lt::mkdir(lt::dirname(merged_file), /*force=*/true);
    TTree::SetMaxTreeSize(5368709120); // 5GB
    TChain* chain = rt::CreateTChainFromCommaSeperatedList(input_path, tree_name);
    chain->Merge(merged_file.c_str(), option.c_str());
    std::cout << "[merge_tchain] complete." << std::endl; 
    delete chain;
}

int main(int argc, char* argv[])
{
    // inputs
    // -----------------------------------------------//

    std::string input_file  = "";
    std::string output_file = "";
    std::string tree_name   = "";
    std::string option      = "fast";

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help"   , "print this menu")
        ("input" , po::value<std::string>(&input_file)->required() , "REQUIRED: name of input file" )
        ("output", po::value<std::string>(&output_file)->required(), "REQUIRED: name of output file")
        ("tree"  , po::value<std::string>(&tree_name)->required()  , "REQUIRED: tree name"          )
        ("option", po::value<std::string>(&option)                 , "options"                      )
        ;

    // parse it
    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) 
        {
            std::cout << desc << "\n";
            return 1;
        }

        po::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\nexiting" << std::endl;
        std::cout << desc << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error!" << "\n";
        return false;
    }

    // do the merging 
    // -----------------------------------------------//
    MergedTChain(input_file, output_file, tree_name, option);

    // done
    return 0;
}
