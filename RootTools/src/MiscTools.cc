#include "AnalysisTools/RootTools/interface/MiscTools.h"
#include "AnalysisTools/LanguageTools/interface/is_zero.h"
#include "AnalysisTools/LanguageTools/interface/is_equal.h"
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

// miscellaneous helper and functions for ROOT related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// c++ includes
#include <stdexcept>
#include <iostream>
#include <vector>

// ROOT includes
#include "TChain.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TFileMerger.h"

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

    // add root files (returns 0 if successful) -- lower case to match the utility name
    int hadd(const std::string& target, const std::vector<std::string>& sources)
    {
        // create the target folder (if it doesn't exist)
        if (!lt::file_exists(lt::dirname(target)))
        {
            lt::mkdir(lt::dirname(target));
        }

        // test if its has valid root files -- if not, then quit
        if(sources.empty())
        {
            std::cout << "[rt::hadd] Error: No sources to process.  Exiting..." << std::endl;
            return 1;
        }

        // build merger
        TFileMerger file_merger;
        for (size_t i = 0, size = sources.size(); i != size; i++) 
        { 
            file_merger.AddFile(sources.at(i).c_str());
        }

        // open output file
        const bool opened = file_merger.OutputFile(target.c_str());
        if (not opened)
        {
            std::cout << "[rt::hadd] Error: opening target failed.  Exiting...\n" << std::endl;
            return 1;
        }

        // merge
        const bool merged = file_merger.Merge();
        if (not merged)
        {
            std::cout << "[rt::hadd] Error: merge failed.  Exiting...\n" << std::endl;
            return 1;
        }

        return 0;
    }

    
    // Add with error
    std::pair<double, double> AddWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2)
    {
        double result = v1.first + v2.first;
        double error  = sqrt(pow(v1.second, 2) + pow(v2.second, 2));
        return std::make_pair(result, error);
    }

    std::pair<float, float> AddWithError(const std::pair<float, float>& v1, const std::pair<float, float>& v2)
    {
        std::pair<double, double> double_v1(v1.first, v1.second);
        std::pair<double, double> double_v2(v2.first, v2.second);
        std::pair<double, double> result = AddWithError(double_v1, double_v2);
        return std::make_pair(static_cast<float>(result.first), static_cast<float>(result.second));
    }

    // Subtract with error
    std::pair<double, double> SubtractWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2)
    {
        double result = v1.first - v2.first;
        double error  = sqrt(pow(v1.second, 2) + pow(v2.second, 2));
        return std::make_pair(result, error);
    }

    std::pair<float, float> SubtractWithError(const std::pair<float, float>& v1, const std::pair<float, float>& v2)
    {
        std::pair<double, double> double_v1(v1.first, v1.second);
        std::pair<double, double> double_v2(v2.first, v2.second);
        std::pair<double, double> result = SubtractWithError(double_v1, double_v2);
        return std::make_pair(static_cast<float>(result.first), static_cast<float>(result.second));
    }

    // Multiply with error
    std::pair<double, double> MultiplyWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2)
    {
        double result = v1.first * v2.first;
        double error  = std::abs(result) * sqrt(pow(v1.second/v1.first, 2) + pow(v2.second/v2.first, 2));
        return std::make_pair(result, error);
    }

    std::pair<float, float> MultiplyWithError(const std::pair<float, float>& v1, const std::pair<float, float>& v2)
    {
        std::pair<double, double> double_v1(v1.first, v1.second);
        std::pair<double, double> double_v2(v2.first, v2.second);
        std::pair<double, double> result = MultiplyWithError(double_v1, double_v2);
        return std::make_pair(static_cast<float>(result.first), static_cast<float>(result.second));
    }

    // Divide with error
    std::pair<double, double> DivideWithError(const std::pair<double, double>& v1, const std::pair<double, double>& v2)
    {
        if (lt::is_zero(v2.first))
        {
            throw std::runtime_error("[rt::DivideWithError] Error: attempting to divide by zero!");
        }
        double result = v1.first / v2.first;
        double error  = std::abs(result) * sqrt(pow(v1.second/v1.first, 2) + pow(v2.second/v2.first, 2));
        return std::make_pair(result, error);
    }

    std::pair<float, float> DivideWithError(const std::pair<float, float>& v1, const std::pair<float, float>& v2)
    {
        std::pair<double, double> double_v1(v1.first, v1.second);
        std::pair<double, double> double_v2(v2.first, v2.second);
        std::pair<double, double> result = DivideWithError(double_v1, double_v2);
        return std::make_pair(static_cast<float>(result.first), static_cast<float>(result.second));
    }

    // set style
    void SetStyle(const std::string& option)
    {
        TStyle *myStyle = gROOT->GetStyle("Plain");
        TGaxis::SetMaxDigits(5);
        myStyle->SetPaintTextFormat("1.3f");
        myStyle->SetOptStat(option.c_str());
        myStyle->SetOptFit(1111);
        myStyle->SetTitleBorderSize(0);
        myStyle->SetTitleFillColor(0);
        myStyle->SetTitleStyle(0);
        //myStyle->SetTitleX( 0.5 - 2.0*myStyle->GetTitleXSize() );
        //myStyle->SetTitleX( 0.05 );
        //myStyle->SetTitleY( 0.96 );
        //myStyle->SetTitleAlign(22);
        myStyle->SetLegendBorderSize(0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,30,0)
        myStyle->SetLegendFillColor(0);
#endif

        //myStyle->SetTitleY(0.985);
        myStyle->SetHistLineColor(kBlue);
        myStyle->SetHistLineWidth(1);
        myStyle->SetPadGridX(1);
        myStyle->SetPadGridY(1);
        myStyle->SetGridColor(kGray+1);
        myStyle->SetCanvasDefW(600);
        myStyle->SetCanvasDefH(600);
        myStyle->SetStatH(0.09);
        myStyle->SetStatW(0.2);
        myStyle->SetStatX(0.99);
        myStyle->SetStatY(0.99);
        myStyle->SetStatBorderSize(myStyle->GetHistLineWidth());
        myStyle->SetStatTextColor(kBlack);
        myStyle->SetStatColor(0);
        //myStyle->SetStatStyle(Style_t style = 1001);
        //void SetStatBoxPosition(TH1* hist_ptr, float x1 = 0.8, float y1 = 0.8, float x2 = 1.0, float y2 = 1.0);

        myStyle->SetTitleOffset(1.2, "Y");
        myStyle->SetLabelSize(0.035, "Y");
        myStyle->SetLabelSize(0.035, "X");
        myStyle->cd();
        gStyle = myStyle;
    }

    // copy the index.php file to dirname
    void CopyIndexPhp(const std::string& target_dir)
    {
        if (!lt::file_exists(target_dir))
        {
            throw std::runtime_error("[rt::CopyIndexPhp] Error : destination directory doesn't exist");
        }
        std::string source = Form("%s/src/AnalysisTools/RootTools/tools/index.php", lt::getenv("CMSSW_BASE").c_str());
        lt::copy_file(source, target_dir + "/index.php");
        return;
    }

} // namespace rt
