#include "AnalysisTools/RootTools/interface/MiscTools.h"
// #include "AnalysisTools/LanguageTools/interface/is_zero.h"
// #include "AnalysisTools/LanguageTools/interface/is_equal.h"
#include "AnalysisTools/LanguageTools/interface/StringTools.h"

// miscellaneous helper and functions for ROOT related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// c++ includes
#include <stdexcept>
#include <iostream>
#include <vector>

// ROOT includes
#include "TChain.h"

// namespace rt --> root tools
namespace rt
{
    // make a chain from a path
	TChain* MakeTChain
	(
		const std::string& glob, 
		const std::string& treename, 
		TChain* const chain_in, 
		const bool verbose
	)
    {
        std::string cmd = "ls ";
        cmd += glob;
        FILE *f = popen(cmd.c_str(), "r");
        if (!f) {
            perror("Opening pipe");
            return 0;
        }
        TChain *c = chain_in == 0 ? new TChain(treename.c_str()) : chain_in;
        int s;
        do {
            char fname[1024];
            s = fscanf(f, " %1024s\n", fname);
            if (s != 1) {
                if (s != EOF)
                    perror("scanning file list");
            } else {
                if (verbose)
                    printf("Adding %s\n", fname);
                c->Add(fname);
            }
        } while (s == 1);
        if (pclose(f) == -1) 
            perror("Closing pipe");
        return c;
    }

    // create a chain from a comma serperated list (e.g. "file1,file2,...")
    // NOTE: user is charge of deleting!
    TChain* CreateTChainFromCommaSeperatedList
    (
        const std::string& str, 
        const std::string& treename, 
        const std::string& prefix
    )
    {
        using namespace std;
        TChain* chain = NULL;
        std::vector<std::string> files = lt::string_split(str, ",");    
        for (size_t i = 0; i != files.size(); i++)
        {
            std::string full_path = prefix+files.at(i);
            if (i == 0)
            {
                chain = MakeTChain(full_path.c_str(), treename.c_str());
            }
            else
            {
                chain = MakeTChain(full_path.c_str(), treename.c_str(), chain);
            }
        }

        // check the chain
        if (!chain)
        {
            throw std::runtime_error("[rt::CreateTChainFromCommaSeperatedList] Error: chain is NULL");
        }
    
        return chain;
    }

    
    // print list of files in a TChain
    void PrintFilesFromTChain(const TChain& chain)
    {
        std::cout << "[rt::PrintFilesFromTChain]:\n" << chain.GetName() << "\t# entries:  " << chain.GetEntries() << std::endl;
        TObjArray* list = chain.GetListOfFiles();
        for (int i = 0; i < list->GetEntries(); i++)
        {
            std::cout << "  " << list->At(i)->GetTitle() << std::endl;
        }
        return;
    }

    // print list of files in a TChain
    void PrintFilesFromTChain(TChain* chain)
    {
        if (!chain)
        {
            std::cout << "[rt::PrintFilesFromTChain] Error: chain is NULL" << std::endl;
            return;
        }
        PrintFilesFromTChain(*chain);
        return;
    }

    // print list of files in a vector of TChain
    void PrintFilesFromTChain(std::vector<TChain*> chains)
    {
        for (std::vector<TChain*>::const_iterator ichain = chains.begin(); ichain != chains.end(); ichain++)
        {
            PrintFilesFromTChain(*ichain);
        }
    }

    // resturn a vector of files in a TChain
    std::vector<std::string> GetFilesFromTChain(TChain* chain)
    {
        if (!chain)
        {
            throw std::runtime_error("[rt::PrintFilesFromTChain] Error: chain is NULL");
        }
        std::vector<std::string> result;
        TObjArray* list = chain->GetListOfFiles();
        for (int i = 0; i < list->GetEntries(); i++)
        {
            result.push_back(list->At(i)->GetTitle());
        }
        return result;
    }

} // namespace rt
