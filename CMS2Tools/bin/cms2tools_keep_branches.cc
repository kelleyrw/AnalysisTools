// C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <regex>

// ROOT includes
#include "TH1.h"
#include "TFile.h"
#include "TChain.h"
#include "TBranch.h"
#include "TSystem.h"

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

// Tools
#include "AnalysisTools/RootTools/interface/RootTools.h"
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

// ------------------------------------------------------------------------------------ //
// helper functions 
// ------------------------------------------------------------------------------------ //

std::string GetBranchNameFromAlias(TTree& tree, const std::string& alias_name)
{
    TNamed * const alias = static_cast<TNamed*>(tree.GetListOfAliases()->FindObject(alias_name.c_str())); 
    const std::string branch_name = lt::string_replace_first(alias->GetTitle(), ".obj", ".*");
    return branch_name;
}

std::vector<std::string> GetListOfAlisesFromTree(TTree& tree)
{
    TList * const alias_list = tree.GetListOfAliases();
    std::vector<std::string> result;
    for (TObjLink *link = alias_list->FirstLink(); link != alias_list->LastLink(); link = link->Next())
    {
        result.push_back(link->GetObject()->GetName());
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> FilterStringVector(const std::vector<std::string>& str_vec, const std::vector<std::string>& patterns)
{
    const std::vector<std::regex> exps(patterns.begin(), patterns.end());
    std::vector<std::string> result;
    for (const auto& str : str_vec)
    {
        for (const auto& exp : exps) 
        {
            if (std::regex_match(str, exp)) result.push_back(str);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------------------------------------ //
// The main program 
// ------------------------------------------------------------------------------------ //

int main(int argc, char **argv)
try
{
    // parse the inputs
    // -------------------------------------------------------------------------------------------------//

    // check that the python is passed
    if (argc < 2)
    {
        throw std::invalid_argument(Form("Usage : %s [parameters.py]", argv[0]));
    }

    // check that pset contains "process" 
    const std::string pset_filename = argv[1];
    if (!edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process"))
    {
        throw std::invalid_argument(Form("[stop_interp_plots] Error: ParametersSet 'process' is missing in your configuration file"));
    }

    // get the python configuration
    const edm::ParameterSet& process = edm::readPSetsFrom(pset_filename)->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& cfg     = process.getParameter<edm::ParameterSet>("cms2tools_drop_branches");

    // get the inputs 
    const long long max_events                           = cfg.getParameter<long long>("max_events");
    const std::vector<std::string> input_files      = cfg.getParameter<std::vector<std::string> >("input_files");
    const std::string tree_name                     = cfg.getParameter<std::string>("tree_name");
    const std::string output_file                   = cfg.getParameter<std::string>("output_file");
    const std::vector<std::string> keep_alias_names = cfg.getParameter<std::vector<std::string> >("keep_alias_names");

    // print the inputs  
    std::cout << "[cms2tools_keep_branches] inputs:\n";
    std::cout << "max_events       = " << max_events                        << "\n";
    std::cout << "input_files      = " << lt::ArrayString(input_files)      << "\n";
    std::cout << "tree_name        = " << tree_name                         << "\n";
    std::cout << "output_file      = " << output_file                       << "\n";
    std::cout << "keep_alias_names = " << lt::ArrayString(keep_alias_names) << "\n";
    std::cout << std::endl;

    // peform the skim
    // -------------------------------------------------------------------------------------------------//

    // FWLite libs
    gSystem->Load("libFWCoreFWLite");
    AutoLibraryLoader::enable();

    // loop over files and make a TTree for each file 
    std::vector<std::string> temp_output_files;
    for (size_t i = 0; i < input_files.size(); ++i)
    {
        TFile old_file(input_files.at(i).c_str());
        TTree * const old_tree = static_cast<TTree*>(old_file.Get(tree_name.c_str()));

        // drop branches
        old_tree->SetBranchStatus("*", 0);

        // keep branches
        std::vector<std::string> all_aliases  = GetListOfAlisesFromTree(*old_tree);
        std::vector<std::string> keep_aliases = FilterStringVector(all_aliases, keep_alias_names);
        for (const auto& alias : keep_aliases)
        {
            const std::string branch_name = GetBranchNameFromAlias(*old_tree, alias);
            old_tree->SetBranchStatus(branch_name.c_str(), 1);
        }

        // clone the tree
        const std::string temp_output_file = Form("%s_%lu.root", lt::filestem(output_file).c_str(), i);
        temp_output_files.push_back(temp_output_file);
        lt::mkdir(lt::dirname(temp_output_file), /*force*/true);
        TFile new_file(temp_output_file.c_str(), "RECREATE");
        TTree * const new_tree = old_tree->CloneTree(max_events, "fast");

        // drop alias
        std::vector<std::string> drop_aliases;
        drop_aliases.reserve(all_aliases.size());
        std::set_difference(all_aliases.begin(), all_aliases.end(), keep_aliases.begin(), keep_aliases.end(), std::back_inserter(drop_aliases));
        for (const auto& alias : drop_aliases)
        {
             TNamed * const alias_ptr = (TNamed*)new_tree->GetListOfAliases()->FindObject(alias.c_str());
             new_tree->GetListOfAliases()->Remove(alias_ptr);
        }

        // write output
        std::cout << "[cms2tools_keep_branches] writing temp file: " << temp_output_file << std::endl;
        new_tree->Write();
    }

    // merge trees together
    std::cout << "[cms2tools_keep_branches] merging temp file to: " << output_file << std::endl;
    int hadd_result = rt::hadd(output_file, temp_output_files);

    // test result
    if (hadd_result==0)
    {
        // delete temp files
        for (const auto& temp_file : temp_output_files)
        {
            lt::remove_file(temp_file);
        }

        // test output
        std::cout << "[cms2tools_keep_branches] branches kept:\n";
        TFile file(output_file.c_str());
        TTree * const tree = static_cast<TTree*>(file.Get(tree_name.c_str()));
        tree->GetListOfBranches()->ls();
        tree->GetListOfAliases()->ls();
    }

    // done
    return 0;
}
catch (std::exception& e)
{
    std::cerr << "[cms2tools_keep_branches] Error: failed..." << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
}
