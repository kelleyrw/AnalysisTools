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
#include "TEventList.h"

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

// Tools
#include "AnalysisTools/RootTools/interface/RootTools.h"
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"
#include "boost/program_options.hpp"

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

    // get the inputs 
    std::string pset_filename = "";
    long long max_events = -1; 
    std::vector<std::string> input_files; 
    std::string tree_name = ""; 
    std::string output_file = ""; 
    std::string selection = ""; 
    std::vector<std::string> keep_alias_names;

    // parse arguments
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help"             , "print this menu")
        ("pset"             , po::value<std::string>(&pset_filename)->required()      , "REQUIRED: python configuration file"      )
        ("max_events"       , po::value<long long>(&max_events)                       , "maximum number of events to skim"         )
        ("input_files"      , po::value<std::vector<std::string> >(&input_files)      , "input ROOT files"                         )
        ("tree_name"        , po::value<std::string>(&tree_name)                      , "name of the TTree"                        )
        ("output_file"      , po::value<std::string>(&output_file)                    , "output ROOT file"                         )
        ("selection"        , po::value<std::string>(&selection)                      , "selection in the form of TTree::Draw/Scan")
        ("keep_alias_names" , po::value<std::vector<std::string> >(&keep_alias_names) , "Regexpression for aliases to keep"        )
        ;
    try
    {
        // first parse command line options
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")) 
        {
            std::cout << desc << "\n";
            return 1;
        }
        po::notify(vm);

        // get the inputs from pset

        // check that pset contains "process" 
        if (!edm::readPSetsFrom(pset_filename)->existsAs<edm::ParameterSet>("process"))
        {
            throw std::invalid_argument(Form("[cms2tools_keep_branches] Error: ParametersSet 'process' is missing in your configuration file"));
        }

        // get the python configuration
        const edm::ParameterSet& process = edm::readPSetsFrom(pset_filename)->getParameter<edm::ParameterSet>("process");
        const edm::ParameterSet& cfg     = process.getParameter<edm::ParameterSet>("cms2tools_drop_branches");

        max_events       = cfg.getParameter<long long>("max_events");
        input_files      = cfg.getParameter<std::vector<std::string> >("input_files");
        tree_name        = cfg.getParameter<std::string>("tree_name");
        output_file      = cfg.getParameter<std::string>("output_file");
        selection        = cfg.getParameter<std::string>("selection");
        keep_alias_names = cfg.getParameter<std::vector<std::string> >("keep_alias_names");

        // now parse command line again to see if there are overrides
        po::store(po::parse_command_line(argc, argv, desc), vm);
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
        return 1;
    }

    // print the inputs  
    std::cout << "[cms2tools_keep_branches] inputs:\n";
    std::cout << "max_events       = " << max_events                        << "\n";
    std::cout << "input_files      = " << lt::ArrayString(input_files)      << "\n";
    std::cout << "tree_name        = " << tree_name                         << "\n";
    std::cout << "output_file      = " << output_file                       << "\n";
    std::cout << "keep_alias_names = " << lt::ArrayString(keep_alias_names) << "\n";
    std::cout << "selection        = " << selection                         << "\n";
    std::cout << std::endl;

    // peform the skim
    // -------------------------------------------------------------------------------------------------//

    // FWLite libs
    gSystem->Load("libFWCoreFWLite");
    AutoLibraryLoader::enable();

    // number of events
    TChain * const chain = rt::CreateTChain(tree_name, input_files);
    input_files = rt::GetFilesFromTChain(chain);
    long long num_events_remaining = (max_events < 0 ? chain->GetEntries() : (max_events > chain->GetEntries() ? chain->GetEntries() : max_events));
    rt::PrintFilesFromTChain(chain);
    std::cout << "[cms2tools_keep_branches] processing " << num_events_remaining << " events\n"; 

    // loop over files and make a TTree for each file 
    std::vector<std::string> temp_output_files;
    for (size_t i = 0; i < input_files.size() && num_events_remaining > 0; ++i)
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
        TTree * const new_tree = old_tree->CopyTree(selection.c_str(), "", num_events_remaining);
        num_events_remaining -= old_tree->GetEntries();
    
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
    int hadd_result = 0;
    if (temp_output_files.size()==1)
    {
        hadd_result = (lt::move_file(temp_output_files.front(), output_file) ? 0 : -1);
    }
    else
    {
        hadd_result = rt::hadd(output_file, temp_output_files);
    }
    
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
