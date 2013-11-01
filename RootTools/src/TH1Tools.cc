#include "AnalysisTools/RootTools/interface/TH1Tools.h"
#include "AnalysisTools/RootTools/interface/MiscTools.h"
#include "AnalysisTools/LanguageTools/interface/is_zero.h"
#include "AnalysisTools/LanguageTools/interface/is_equal.h"
#include "AnalysisTools/LanguageTools/interface/StringTools.h"
#include "AnalysisTools/LanguageTools/interface/OSTools.h"

// helper and functions for ROOT TH1 related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// c++ includes
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <map>

// ROOT includes
#include "TCanvas.h"
#include "TPad.h"
#include "TPaveStats.h"

// namespace rt --> root tools
namespace rt
{
    // get a root file (throws if file not found)
    TFile* OpenRootFile(const std::string& file_name, const std::string& option)
    {
        TFile* file = TFile::Open(file_name.c_str(), option.c_str());
        if (!file || file->IsZombie()) 
        {
            throw std::runtime_error(std::string("[rt::OpenRootFile] Error: '") + file_name + "' not found or empty.  Aborting!");
        }
        return file;
    }

    // get all the hists from a root file (given a file) in a map of hist pointers 
    // (give empty map if dir is incorrect or emptry)
    // (throws if file not found)
    std::map<std::string, TH1*> GetMapOfTH1s(TFile* const file, const std::string& root_file_dir)
    {
        using namespace std;

        if (!file || file->IsZombie()) 
        {
            throw std::runtime_error(string("[rt::GetMapOfTH1s] Error: '") + file->GetName() + "' not found or empty.  Aborting!");
        }

        map<string, TH1*> hist_map;
        if (!file->cd(root_file_dir.c_str()))
        {
            cout << "[rt::GetMapOfTH1s] Warning: '" << root_file_dir 
                << " is not in the ROOT file (" << file->GetName() 
                << "). Returning empty map of hists" << endl;
            return hist_map;
        }
        for (TObjLink* link = gDirectory->GetListOfKeys()->FirstLink(); link != NULL; link = link->Next())
        {
            if (TH1* hist_ptr = dynamic_cast<TH1*>(gDirectory->Get(link->GetObject()->GetName())))
            {
                // non interactive, I want to be in charge of deleting
                hist_ptr->SetDirectory(0);
                string name(hist_ptr->GetName());
                hist_map.insert(pair<string, TH1*>(name, hist_ptr));
            }
        }
        return hist_map;
    }

    // get all the hists from a root file (given a filename) in a map of hist pointers 
    // (give empty map if dir is incorrect or emptry)
    // (throws if file not found)
    std::map<std::string, TH1*> GetMapOfTH1s(const std::string& filename, const std::string& root_file_dir)
    {
        TFile* file = rt::OpenRootFile(filename);
        return rt::GetMapOfTH1s(file, root_file_dir);
    }

    // get all the hists from a root file (given a file) in a map of hist pointers 
    // (give empty map if dir is incorrect or emptry)
    // (throws if file not found)
    std::vector<TH1*> GetVectorOfTH1s(TFile* const file, const std::string& root_file_dir)
    {
        using namespace std;

        if (file->IsZombie())
        {
            throw std::runtime_error(string("[rt::GetVectorOfTH1s] Error: '") + file->GetName() + "' not found or empty.  Aborting!");
        }

        vector<TH1*> hist_vec;
        if (!file->cd(root_file_dir.c_str()))
        {
            cout << "[rt::GetVectorOfTH1s] Warning: '" << root_file_dir 
                << " is not in the ROOT file (" << file->GetName() 
                << "). Returning empty vector of hists" << endl;
            return hist_vec;
        }
        for (TObjLink* link = gDirectory->GetListOfKeys()->FirstLink(); link != NULL; link = link->Next())
        {
            if (TH1* hist_ptr = dynamic_cast<TH1*>(gDirectory->Get(link->GetObject()->GetName())))
            {
                // non interactive, I want to be in charge of deleting
                hist_ptr->SetDirectory(0);
                hist_vec.push_back(hist_ptr);
            }
        }
        return hist_vec;
    }

    // get all the hists from a root file (given a filename) in a vector of hist pointers 
    // (give empty map if dir is incorrect or emptry)
    // (throws if file not found)
    std::vector<TH1*> GetVectorOfTH1s(const std::string& filename, const std::string& root_file_dir)
    {
        TFile* file = rt::OpenRootFile(filename);
        return rt::GetVectorOfTH1s(file, root_file_dir);
    }

    // save the contents of the map to a root file
    void Write
    (
        std::map<std::string, TH1*>& hist_map, 
        const std::string& file_name, 
        const std::string& root_file_dir,
        const std::string& option
    )
    {
        lt::mkdir(lt::dirname(file_name), /*recursive=*/true);

        TFile* output_file = TFile::Open(file_name.c_str(), option.c_str());
        output_file->cd("");
        if (!output_file->cd(root_file_dir.c_str()))
        {
            output_file->mkdir(root_file_dir.c_str());
        }
        output_file->cd(root_file_dir.c_str());

        for (std::map<std::string, TH1*>::iterator itr = hist_map.begin(); itr != hist_map.end(); itr++)
        {
            itr->second->Write(itr->first.c_str(), TObject::kOverwrite);
        }
        output_file->Close();
        return;
    }

    void Write
    (
         std::map<std::string, TH1*>& hist_map, 
         TFile* const root_file,
         const std::string& root_file_dir
    )
    {
        if (root_file->IsZombie()) 
        {
            throw std::runtime_error(std::string("[rt::Write] Error: '") + root_file->GetName() + "' not found or empty.  Aborting!");
        }

        root_file->cd("");
        if (!root_file->cd(root_file_dir.c_str()))
        {
            root_file->mkdir(root_file_dir.c_str());
        }
        root_file->cd(root_file_dir.c_str());

        for (std::map<std::string, TH1*>::iterator itr = hist_map.begin(); itr != hist_map.end(); itr++)
        {
            itr->second->Write(itr->first.c_str(), TObject::kOverwrite);
        }
        return;
    }


    // save the contents of the vector to a root file
    void Write
    (
        std::vector<TH1*>& hist_vector, 
        const std::string& file_name, 
        const std::string& root_file_dir,
        const std::string& option
    )
    {
        lt::mkdir(lt::dirname(file_name), /*recursive=*/true);

        TFile* output_file = TFile::Open(file_name.c_str(), option.c_str());
        output_file->cd("");
        if (!output_file->cd(root_file_dir.c_str()))
        {
            output_file->mkdir(root_file_dir.c_str());
        }
        output_file->cd(root_file_dir.c_str());

        for (std::vector<TH1*>::iterator itr = hist_vector.begin(); itr != hist_vector.end(); itr++)
        {
            (*itr)->Write((*itr)->GetName(), TObject::kOverwrite);
        }
        output_file->Close();
        return;
    }

    // save a histogram to a file 
    void Write
    (
        const TH1* const hist_ptr, 
        const std::string& file_name, 
        const std::string& root_file_dir,
        const std::string& option
    )
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::Write] Warning: histogram pointer is NULL!");
        }

        lt::mkdir(lt::dirname(file_name), /*recursive=*/true);
        TFile* output_file = TFile::Open(file_name.c_str(), option.c_str());
        output_file->cd("");
        if (!output_file->cd(root_file_dir.c_str()))
        {
            output_file->mkdir(root_file_dir.c_str());
        }
        output_file->cd(root_file_dir.c_str());

        hist_ptr->Write(hist_ptr->GetName(), TObject::kOverwrite);
        output_file->Close();
        return;
    }

    // print the histogram to eps/png/pdf 
    void Print
    (
        TH1* const hist_ptr, 
        const std::string& dir_name, 
        const std::string& suffix,
        const std::string& file_name,
        const std::string& option,
        const bool logy
    )
    {
        if (not(suffix == "eps" || suffix == "png" || suffix == "pdf"))
        {
            std::cout << "suffix " << suffix << " not valid!  No print." << std::endl;
            return;
        }

        TCanvas c1("c1_Print_temp", "c1_Print_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(dir_name, /*recursive=*/true);

        if (!hist_ptr)
        {
            std::cout << "rt::Print() Error: hist_ptr is NULL" << std::endl;
            return;
        }
        hist_ptr->Draw(option.c_str());
        std::string output_file_name = "";
        if (file_name.empty())
        {
            output_file_name = Form("%s/%s.%s", dir_name.c_str(), hist_ptr->GetName(), suffix.c_str());
        }
        else
        {
            output_file_name = Form("%s/%s.%s", dir_name.c_str(), file_name.c_str(), suffix.c_str());
        }
        c1.Print(output_file_name.c_str());
        rt::CopyIndexPhp(dir_name);
        return;
    }

    // print the map of histograms to eps/png/pdf 
    void Print
    (
        std::map<std::string, TH1*>& hist_map, 
        const std::string& dir_name, 
        const std::string& suffix,
        const std::string& option,
        const bool logy
    )
    {
        if (not(suffix == "eps" || suffix == "png" || suffix == "pdf"))
        {
            std::cout << "suffix " << suffix << " not valid!  No print." << std::endl;
            return;
        }

        TCanvas c1("c1_Print_temp", "c1_Print_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(dir_name, /*recursive=*/true);

        for (std::map<std::string, TH1*>::const_iterator itr = hist_map.begin(); itr != hist_map.end(); itr++)
        {
            if (!itr->second)
            {
                std::cout << "rt::Print() Warning: Object associated to " << itr->first << " is NULL -- skipping!" << std::endl;
                continue;
            }
            itr->second->Draw(option.c_str());
            c1.Print((dir_name + "/" + itr->first + "." + suffix).c_str());
        }
        rt::CopyIndexPhp(dir_name);
        return;
    }

    // print the vector of histograms to eps/png/pdf 
    void Print
    (
        std::vector<TH1*>& hist_vector, 
        const std::string& dir_name, 
        const std::string& suffix,
        const std::string& option,
        const bool logy
    )
    {
        if (not(suffix == "eps" || suffix == "png" || suffix == "pdf"))
        {
            std::cout << "suffix " << suffix << " not valid!  No print." << std::endl;
            return;
        }

        TCanvas c1("c1_Print_temp", "c1_Print_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(dir_name, /*recursive=*/true);

        for(size_t i = 0; i < hist_vector.size(); i++)
        {
            if (!hist_vector.at(i))
            {
                std::cout << "rt::Print() Warning: Object is NULL -- skipping! " << std::endl;
                continue;
            }
            hist_vector.at(i)->Draw(option.c_str());
            c1.Print((dir_name + "/" +  hist_vector.at(i)->GetName() + "." + suffix).c_str());
        }
        rt::CopyIndexPhp(dir_name);
        return;
    }
    

    // make an efficiency plot by dividing the two histograms 
    TH1* MakeEfficiencyPlot(TH1* num_hist, TH1* den_hist, const std::string& name, const std::string& title)
    {
        using namespace std;

        // check that hists are valid
        if (!num_hist || !den_hist)
        {
            throw std::runtime_error("[rt::MakeEfficiencyPlot] Error: one of the Histograms are NULL");
        }

        // verify that all histograms have same binning
        if (den_hist->GetNbinsX() != num_hist->GetNbinsX()) 
        {
            throw std::runtime_error("[rt::MakeEfficiencyPlot] Error: Histograms must have same number of bins");
        }

        // get the new histogram
        TH1F* temp = (TH1F*) num_hist->Clone(name.c_str());
        temp->SetTitle(title.empty() ? name.c_str() : title.c_str());
        temp->Reset();
        if (!temp->GetSumw2N())
        {
            temp->Sumw2();
        }

        // Do the calculation
        temp->Divide(num_hist, den_hist, 1.0, 1.0, "B");

        // Done
        return temp;
    }

    // make a 2D efficiency plot by dividing the two histograms 
    TH2* MakeEfficiencyPlot2D(TH1* num_hist, TH1* den_hist, const std::string& name, const std::string& title)
    {
        TH2* num_hist_temp = dynamic_cast<TH2*>(num_hist);
        TH2* den_hist_temp = dynamic_cast<TH2*>(den_hist);

        // check that hists are valid after cast
        if (!num_hist_temp || !den_hist_temp)
        {
            throw std::runtime_error("[rt::MakeEfficiencyPlot2D] Error: one of the Histograms are NULL or not castable to a TH2*");
        }
        return MakeEfficiencyPlot2D(num_hist_temp, den_hist_temp, name, title);
    }

    TH2* MakeEfficiencyPlot2D(TH2* num_hist, TH2* den_hist, const std::string& name, const std::string& title)
    {
        // check that hists are valid
        if (!num_hist || !den_hist)
        {
            throw std::runtime_error("[rt::MakeEfficiencyPlot2D] Error: one of the Histograms are NULL");
        }

        // verify that all histograms have same binning
        if ((den_hist->GetNbinsX() != num_hist->GetNbinsX()) && (den_hist->GetNbinsY() != num_hist->GetNbinsY())) 
        {
            throw std::runtime_error("[rt::MakeEfficiencyPlot2D] Error: Histograms must have same number of bins");
        }

        // get the new histogram
        TH2F* temp = dynamic_cast<TH2F*>(num_hist->Clone(name.c_str()));
        temp->SetTitle(title.empty() ? name.c_str() : title.c_str());
        temp->Reset();
        if (!temp->GetSumw2N())
        {
            temp->Sumw2();
        }

        // Do the calculation
        temp->Divide(num_hist, den_hist, 1.0, 1.0, "B");

        // Done
        return temp;
    }

    // create a Projection from 2D hist 
    TH1* MakeProjectionPlot
    (
        TH2* const hist, 
        const std::string& axis, 
        const std::string& name, 
        const std::string& title, 
        double low, 
        double high,
        const std::string& option
    )
    {
        using namespace std;

        // check that hists are valid
        if (!hist)
        {
            throw std::runtime_error("[rt::MakeProjectionPlot] Error: the Histogram is NULL");
        }

        // check that axis is setup to an one of the apppriate values
        string axis_name = lt::string_lower(axis);
        if (axis_name != "x" && axis_name != "y") 
        {
            throw std::runtime_error("[rt::MakeProjectionPlot] Error: Invalid axis choice (\"X\" or \"Y\")");
        }

        // get the appropriate bins
        TAxis* xaxis = (axis_name=="y") ? hist->GetYaxis() : hist->GetXaxis();
        TAxis* yaxis = (axis_name=="y") ? hist->GetXaxis() : hist->GetYaxis();
        int first_bin = 0;
        int last_bin  = yaxis->GetNbins()+1;
        if (high > low)
        {
            first_bin = yaxis->FindBin(low+0.00001);    
            last_bin  = yaxis->FindBin(high-0.00001);    
        }

        string hist_name  = name.empty() ? string(hist->GetName()) + "_proj" + axis : name;
        string hist_title = title.empty() ? string(hist->GetTitle()) + " " + axis + " projection" : title;
        TH1* result       = (axis_name=="y") ? hist->ProjectionY(hist_name.c_str(), first_bin, last_bin, option.c_str()) : 
                                               hist->ProjectionX(hist_name.c_str(), first_bin, last_bin, option.c_str());
        result->SetNameTitle(hist_name.c_str(), hist_title.c_str());
        result->GetXaxis()->ImportAttributes(xaxis);
        result->GetXaxis()->SetTitle(xaxis->GetTitle());
        if (!hist->GetSumw2N())
        {
            hist->Sumw2();
        }
        return result;
    }

    // create a Projection from num and den hists 
    TH1* MakeEfficiencyProjectionPlot
    (
        TH2* const num_hist, 
        TH2* const den_hist, 
        const std::string& axis, 
        const std::string& name, 
        const std::string& title, 
        double low, 
        double high,
        const std::string& option
    )
    {
        using namespace std;

        // check that hists are valid
        if (!num_hist || !den_hist)
        {
            throw std::runtime_error("[rt::MakeEfficiencyProjectionPlot] Error: one of the Histograms are NULL");
        }

        // check that axis is setup to an one of the apppriate values
        string axis_name = lt::string_lower(axis);
        if (axis_name != "x" && axis_name != "y") 
        {
            throw std::runtime_error("[rt::MakeEfficiencyProjectionPlot] Error: Invalid axis choice (\"X\" or \"Y\")");
        }

        TH1* h_num = MakeProjectionPlot(num_hist, axis_name, Form("%s_num", name.c_str()), title, low, high, option.c_str());  
        TH1* h_den = MakeProjectionPlot(den_hist, axis_name, Form("%s_den", name.c_str()), title, low, high, option.c_str());  
        TH1* h_eff = MakeEfficiencyPlot(h_num, h_den, name, title);  
        h_eff->SetNameTitle(name.c_str(), title.empty() ? name.c_str() : title.c_str());
        delete h_num;
        delete h_den;
        return h_eff;
    }

    // create a Projection from num and den hists 
    // this version for convenience (num and den hist must be castable to TH2*)
    TH1* MakeEfficiencyProjectionPlot
    (
        TH1* num_hist, 
        TH1* den_hist, 
        const std::string& axis, 
        const std::string& name, 
        const std::string& title, 
        double low, 
        double high,
        const std::string& option
    )
    {
        TH2* num_hist_temp = dynamic_cast<TH2*>(num_hist);
        TH2* den_hist_temp = dynamic_cast<TH2*>(den_hist);

        // check that hists are valid after cast
        if (!num_hist_temp || !den_hist_temp)
        {
            throw std::runtime_error("[rt::MakeEfficiencyProjectionPlot] Error: one of the Histograms are NULL or not castable to a TH2*");
        }
        return MakeEfficiencyProjectionPlot(num_hist_temp, den_hist_temp, axis, name, title, low, high, option);
    }

    // Add Hists and return new hist (client is owner of the TH1*)
    TH1* AddHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title)
    {
        // check that hists are valid
        if (!h1 || !h2)
        {
            throw std::runtime_error("[rt::AddHists] Error: at least one of the Histograms are NULL");
        }

        TH1* h_result = dynamic_cast<TH1*>(h1->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Add(h1, h2, 1.0, 1.0);
        return h_result;
    }

    // Add Hists and return new hist (client is owner of the TH1*)
    TH1* AddHists(TH1* const h1, TH1* const h2, TH1* const h3, const std::string& name, const std::string& title)
    {
        // check that hists are valid
        if (!h1 || !h2 || !h3)
        {
            throw std::runtime_error("[rt::AddHists] Error: at least one of the Histograms are NULL");
        }

        TH1* h_result = dynamic_cast<TH1*>(h1->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Add(h1, h2, 1.0, 1.0);
        h_result->Add(h3, 1.0);
        return h_result;
    }

    // Subtract Hists and return new hist (client is owner of the TH1*)
    TH1* SubtractHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title)
    {
        // check that hists are valid
        if (!h1 || !h2)
        {
            throw std::runtime_error("[rt::SubtractHists] Error: at least one of the Histograms are NULL");
        }

        TH1* h_result = dynamic_cast<TH1*>(h1->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Add(h1, h2, 1.0, -1.0);
        return h_result;
    }

    // Divide Hists and return new hist (client is owner of the TH1*)
    TH1* DivideHists(TH1* const h_num, TH1* const h_den, const std::string& name, const std::string& title, const std::string& option)
    {
        // check that hists are valid
        if (!h_num || !h_den)
        {
            throw std::runtime_error("[rt::DivideHists] Error: at least one of the Histograms are NULL");
        }

        TH1* h_result = dynamic_cast<TH1*>(h_num->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Divide(h_num, h_den, 1.0, 1.0, option.c_str());
        return h_result;
    }

    // Muilplty Hists and return new hist (client is owner of the TH1*)
    TH1* MultiplyHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title, const std::string& option)
    {
        // check that hists are valid
        if (!h1 || !h2)
        {
            throw std::runtime_error("[rt::DivideHists] Error: at least one of the Histograms are NULL");
        }

        TH1* h_result = dynamic_cast<TH1*>(h1->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Multiply(h1, h2, 1.0, 1.0, option.c_str());
        return h_result;
    }

    // Subtract Hists and then divide by the first and return new hist (client is owner of the TH1*)
    // rel diff = (h1 - h2)/h1
    TH1* RelativeDiffHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title)
    {
        // check that hists are valid
        if (!h1 || !h2)
        {
            throw std::runtime_error("[rt::RelativeDiffHist] Error: at least one of the Histograms are NULL");
        }

        TH1* h_unity  = dynamic_cast<TH1*>(h1->Clone("h_unity"));
        if (not h_unity->GetSumw2())
        {
            h_unity->Sumw2();
        }
        h_unity->Divide(h_unity, h_unity, 1.0, 1.0, "B"); // zero outs the error
        TH1* h_result = dynamic_cast<TH1*>(h1->Clone(name.c_str()));
        if (not title.empty())
        {
            h_result->SetTitle(title.c_str());
        }
        h_result->Divide(h2, h1, 1.0, 1.0);
        h_result->Add(h_unity, h_result, 1.0, -1.0);
        return h_result;
    }

    // mask off all values that are in the range and set the values to zero 
    void MaskHist2D(TH1* hist, std::string axis_label, const float low, const float high)
    {
        // check that hists are valid
        if (!hist)
        {
            throw std::runtime_error("[rt::MaskHist2D] Error: Histograms are NULL");
        }
        if (!dynamic_cast<TH2*>(hist))
        {
            throw std::runtime_error("[rt::MaskHist2D] Error: Histograms is not 2D");
        }

        // check that axis is setup to an one of the apppriate values
        axis_label = lt::string_lower(axis_label);
        if (axis_label != "x" && axis_label != "y") 
        {
            throw std::runtime_error("[rt::MaskHist2D] Error: Invalid axis choice (\"X\" or \"Y\")");
        }

        for (int xbin = 1; xbin != hist->GetXaxis()->GetNbins()+1; xbin++)
        {
            const float xvalue = hist->GetXaxis()->GetBinCenter(xbin);
            for (int ybin = 1; ybin != hist->GetYaxis()->GetNbins()+1; ybin++)
            {
                const float yvalue = hist->GetYaxis()->GetBinCenter(ybin);
                if (axis_label == "x" && low <= xvalue && xvalue <= high)
                {
                    hist->SetBinContent(xbin, ybin, 0.0f);
                    hist->SetBinError(xbin, ybin, 0.0f);
                }
                if (axis_label == "y" && low <= yvalue && yvalue <= high)
                {
                    hist->SetBinContent(xbin, ybin, 0.0f);
                    hist->SetBinError(xbin, ybin, 0.0f);
                }
            }
        }
        return;
    }

    // maximum value in hist 
    double GetHistMaximumValue(TH1* const hist_ptr, const bool include_error)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetHistMaximumValue] Error: histogram pointer is NULL!");
        }
        double result = hist_ptr->GetMaximum();
        if (include_error)
        {
            result += hist_ptr->GetBinError(hist_ptr->GetMaximumBin());
        }
        return result;
    }

    // minimum value in hist 
    double GetHistMinimumValue(TH1* const hist_ptr, const bool include_error)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetHistMaximumValue] Error: histogram pointer is NULL!");
        }
        double result = hist_ptr->GetMinimum();
        if (include_error)
        {
            result -= hist_ptr->GetBinError(hist_ptr->GetMinimumBin());
        }
        return result >= 0.0 ? result : 0.0; // what is a negative histogram value?
    }

    // normalize the area of the histogram to norm 
    void Normalize(TH1* const hist, const double norm)
    {
        const float integral = static_cast<float>(hist->Integral());
        if (!lt::is_zero(integral))
        {
            hist->Scale(norm/integral);
        }
    }

    // calculate the integral 1D hist
    double Integral(TH1* const hist_ptr, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::Integral] Error: hist pointer is NULL!");
        }
        int max_bin = hist_ptr->GetNbinsX()+1;
        int min_bin = 0;
        return hist_ptr->Integral(min_bin, max_bin, option.c_str());
    }

    // calculate the integral 1D over a range
    double Integral(TH1* const hist_ptr, const float low, const float high, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::Integral] Error: hist pointer is NULL!");
        }
        if (low > high)
        {
            throw std::runtime_error("[rt::Integral] Error: low must > high!");
        }
        int left_bin  = hist_ptr->FindBin(low);
        int right_bin = hist_ptr->FindBin(high);
        if (lt::is_equal(static_cast<double>(high), hist_ptr->GetXaxis()->GetBinLowEdge(right_bin)))
        {
            right_bin--;
        }
        return hist_ptr->Integral(left_bin, right_bin, option.c_str());
    }

    // calculate the integral 2D hist
    double Integral(TH2* const hist_ptr, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::Integral] Error: hist pointer is NULL!");
        }
        int xmax_bin = hist_ptr->GetNbinsX()+1;
        int xmin_bin = 0;
        int ymax_bin = hist_ptr->GetNbinsY()+1;
        int ymin_bin = 0;
        return hist_ptr->Integral(xmin_bin, xmax_bin, ymin_bin, ymax_bin, option.c_str());
    }

    // calculate the integral 2D over a range
    double Integral(TH2* const hist_ptr, const float xlow, const float xhigh, const float ylow, const float yhigh, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::Integral] Error: hist pointer is NULL!");
        }
        if ((xlow > xhigh) || (ylow > yhigh))
        {
            throw std::runtime_error("[rt::Integral] Error: low must > high!");
        }
        int left_bin   = hist_ptr->GetXaxis()->FindBin(xlow);
        int right_bin  = hist_ptr->GetXaxis()->FindBin(xhigh);
        int bottom_bin = hist_ptr->GetYaxis()->FindBin(ylow);
        int top_bin    = hist_ptr->GetYaxis()->FindBin(yhigh);
        if (lt::is_equal(static_cast<double>(xhigh), hist_ptr->GetXaxis()->GetBinLowEdge(right_bin)))
        {
            right_bin--;
        }
        if (lt::is_equal(static_cast<double>(yhigh), hist_ptr->GetYaxis()->GetBinLowEdge(top_bin)))
        {
            top_bin--;
        }
        return hist_ptr->Integral(left_bin, right_bin, bottom_bin, top_bin, option.c_str());
    }

    // find the bin content  
    double GetBinContent1D(TH1* const hist_ptr, const float x)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetBinContent1D] Error: hist pointer is NULL!");
        }
        int bin = hist_ptr->GetXaxis()->FindBin(x);
        return hist_ptr->GetBinContent(bin);
    }

    //  find the bin content 
    double GetBinContent2D(TH2* const hist_ptr, const float x, const float y)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetBinContent1D] Error: hist pointer is NULL!");
        }
        int xbin = hist_ptr->GetXaxis()->FindBin(x);
        int ybin = hist_ptr->GetYaxis()->FindBin(y);
        return hist_ptr->GetBinContent(xbin, ybin);
    }

    // find the bin error 
    double GetBinError1D(TH1* const hist_ptr, const float x)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetBinError1D] Error: hist pointer is NULL!");
        }
        int bin = hist_ptr->GetXaxis()->FindBin(x);
        return hist_ptr->GetBinError(bin);
    }

    //  find the bin error
    double GetBinError2D(TH2* const hist_ptr, const float x, const float y)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::GetBinError1D] Error: hist pointer is NULL!");
        }
        int xbin = hist_ptr->GetXaxis()->FindBin(x);
        int ybin = hist_ptr->GetYaxis()->FindBin(y);
        return hist_ptr->GetBinError(xbin, ybin);
    }

    // set the bin content  
    void SetBinContent1D(TH1* const hist_ptr, const float x, const float value)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent1D]: Error: hist pointer is NULL!");
        }
        int bin = hist_ptr->GetXaxis()->FindBin(x);
        hist_ptr->SetBinContent(bin, value);
    }

    // set the bin content  
    void SetBinContent1D(TH1* const hist_ptr, const float x, const float value, const float error)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent1D]: Error: hist pointer is NULL!");
        }
        int bin = hist_ptr->GetXaxis()->FindBin(x);
        hist_ptr->SetBinContent(bin, value);
        hist_ptr->SetBinError(bin, error);
    }

    // set the bin content 
    void SetBinContent2D(TH1* const hist_ptr, const float x, const float y, const float value)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is NULL!");
        }
        if (TH2* h = dynamic_cast<TH2*>(hist_ptr))
        {
            int xbin = h->GetXaxis()->FindBin(x);
            int ybin = h->GetYaxis()->FindBin(y);
            h->SetBinContent(xbin, ybin, value);
        }
        else
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is not 2D!");
        }
    }

    // set the bin content 
    void SetBinContent2D(TH1* const hist_ptr, const float x, const float y, const float value, const float error)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is NULL!");
        }
        if (TH2* h = dynamic_cast<TH2*>(hist_ptr))
        {
            int xbin = h->GetXaxis()->FindBin(x);
            int ybin = h->GetYaxis()->FindBin(y);
            h->SetBinContent(xbin, ybin, value);
            h->SetBinError(xbin, ybin, error);
        }
        else
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is not 2D!");
        }
    }

    // set the bin content 
    void SetBinContent2D(TH2* const hist_ptr, const float x, const float y, const float value)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is NULL!");
        }
        int xbin = hist_ptr->GetXaxis()->FindBin(x);
        int ybin = hist_ptr->GetYaxis()->FindBin(y);
        hist_ptr->SetBinContent(xbin, ybin, value);
    }

    // set the bin content 
    void SetBinContent2D(TH2* const hist_ptr, const float x, const float y, const float value, const float error)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::SetBinContent2D] Error: hist pointer is NULL!");
        }
        int xbin = hist_ptr->GetXaxis()->FindBin(x);
        int ybin = hist_ptr->GetYaxis()->FindBin(y);
        hist_ptr->SetBinContent(xbin, ybin, value);
        hist_ptr->SetBinError(xbin, ybin, error);
    }

    // calculate integral{low, high} of histogram / total integral of histogram
    double CalcPercentOfIntegral(TH1* const hist_ptr, const float low, const float high, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::CalcPercentOfIntegral] Error: hist pointer is NULL!");
        }
        return Integral(hist_ptr, low, high, option.c_str())/Integral(hist_ptr, option.c_str());
    }

    // set statbox position
    void SetStatBoxPosition(TH1* const hist_ptr, const float x1, const float y1,  float x2, float y2)
    {
        if (!hist_ptr)
        {
            std::cerr << "[rt::SetStatBosPosition] Warning: hist pointer is NULL! Doing nothing." << std::endl;
            return;
        }

        hist_ptr->SetStats(true);
        hist_ptr->Draw(); // make sure the stat box exists
        gPad->Update();
        if (TPaveStats *statbox = dynamic_cast<TPaveStats*>(hist_ptr->FindObject("stats")))
        {
            statbox->SetLineColor(hist_ptr->GetLineColor());
            statbox->SetLineWidth(hist_ptr->GetLineWidth());
            statbox->SetLineStyle(hist_ptr->GetLineStyle());

            statbox->SetX1NDC(x1);
            statbox->SetY1NDC(y1);
            statbox->SetX2NDC(x2);
            statbox->SetY2NDC(y2);
        }
    }

    // a fix to the issue of the erros on the profile hists
    // (is this still an issue?)
    void CalcErrorsForProfile(TProfile* const profile)
    {
        if (!profile)
        {
            std::cout << "[rt::CalcErrorsForProfile] Warning: profile pointer is NULL -- skipping! " << std::endl;
            return;
        }
        profile->SetErrorOption(" ");
        for(size_t i = 0; i < profile->GetEntries(); i++)
        {
            const double err = profile->GetBinError(i);
            const double N   = profile->GetEntries();
            profile->SetBinError(i, err/sqrt(N));
        }
        return;
    }

    // create a CTable from a hist (1D and 2D only)                            
    CTable CreateTableFromHist                                                 
    (                                                                          
        TH1* const hist,                                                             
        const std::string& title,                                              
        const std::string& xtitle,                                             
        const std::string& ytitle,                                             
        const std::string& xunit,                                              
        const std::string& yunit,                                              
        const std::string& precision,                                          
        const std::string& xprecision,                                         
        const std::string& yprecision,                                         
        const bool reverse_axis                                                
    )                                                                          
    {                                                                          
        using std::string;                                                     
                                                                               
        CTable table;                                                          
        if (not title.empty())                                                 
        {                                                                      
            table.useTitle();                                                  
            table.setTitle(title);                                             
        }                                                                      
                                                                               
        // NULL pointer --> throw exception                                    
        if (hist == NULL)                                                      
        {                                                                      
            throw std::invalid_argument("[CreateTableFromHist] Error: hist pointer is NULL"); 
        }                                                                      
                                                                               
        // 3D hist --> throw exception                                         
        if (dynamic_cast<TH3*>(hist))                                          
        {                                                                      
            throw std::invalid_argument("[CreateTableFromHist] Error: only supports TH2 or TH1"); 
        }                                                                      
                                                                               
        // 2D hist                                                             
        if (dynamic_cast<TH2*>(hist))                                          
        {                                                                      
            TAxis* const xaxis = (reverse_axis ? hist->GetYaxis() : hist->GetXaxis());
            TAxis* const yaxis = (reverse_axis ? hist->GetXaxis() : hist->GetYaxis());
                                                                               
            for (int xbin = 1; xbin != xaxis->GetNbins()+1; xbin++)            
            {                                                                  
                const int col = xbin-1;                                        
                const float xmin = xaxis->GetBinLowEdge(xbin);                 
                const float xmax = xaxis->GetBinUpEdge(xbin);                  
                                                                               
                // column label                                                
                string xlabel;                                                 
                if (not xtitle.empty())                                        
                {                                                              
                    if (not xunit.empty())                                     
                    {                                                          
                        string format = "%" + xprecision + "f " + xunit + " $<$ s $<$ %" + xprecision + "f " + xunit;
                        xlabel = Form(format.c_str(), xmin, xtitle.c_str(), xmax);
                    }                                                          
                    else                                                       
                    {                                                          
                        string format = "%" + xprecision + "f $<$ %s $<$ %" + xprecision + "f";
                        xlabel = Form(format.c_str(), xmin, xtitle.c_str(), xmax);
                    }                                                          
                }                                                              
                else                                                           
                {                                                              
                    string format = "%" + xprecision + "f $<$ %s $<$ %" + xprecision + "f";
                    xlabel = Form(format.c_str(), xmin, xaxis->GetTitle(), xmax);
                }                                                              
                table.setColLabel(xlabel, col);                                
                                                                               
                for (int ybin = 1; ybin != yaxis->GetNbins()+1; ybin++)        
                {                                                              
                    const int row = ybin-1;                                    
                    const float ymin = yaxis->GetBinLowEdge(ybin);             
                    const float ymax = yaxis->GetBinUpEdge(ybin);              
                                                                               
                    // row label                                               
                    string ylabel;                                             
                    if (not ytitle.empty())                                    
                    {                                                          
                        if (not yunit.empty())                                 
                        {                                                      
                            string format = "%" + yprecision + "f " + yunit + " $<$ %s $<$ %" + yprecision + "f " + yunit;
                            ylabel = Form(format.c_str(), ymin, ytitle.c_str(), ymax);
                        }                                                      
                        else                                                   
                        {                                                      
                            string format = "%" + yprecision + "f $<$ %s $<$ %" + yprecision + "f";
                            ylabel = Form(format.c_str(), ymin, ytitle.c_str(), ymax);
                        }                                                      
                    }                                                          
                    else                                                       
                    {                                                          
                        string format = "%" + yprecision + "f $<$ %s $<$ %" + yprecision + "f";
                        ylabel = Form(format.c_str(), ymin, yaxis->GetTitle(), ymax);
                    }                                                          
                    table.setRowLabel(ylabel, row);                            
                                                                               
                    // set the cell                                            
                    string entry = (reverse_axis ? lt::pm(hist->GetBinContent(ybin, xbin), hist->GetBinError(ybin, xbin), precision) :
                                                   lt::pm(hist->GetBinContent(xbin, ybin), hist->GetBinError(xbin, ybin), precision));
                    table.setCell(entry, row, col);                            
                }                                                              
            }                                                                  
        }                                                                      
                                                                               
        // 1D hist                                                             
        else if (dynamic_cast<TH1*>(hist))                                     
        {                                                                      
            TAxis* const xaxis = hist->GetXaxis();                             
                                                                               
            for (int xbin = 1; xbin != xaxis->GetNbins()+1; xbin++)            
            {                                                                  
                const int row = xbin-1;                                        
                const float xmin = xaxis->GetBinLowEdge(xbin);                 
                const float xmax = xaxis->GetBinUpEdge(xbin);                  
                                                                               
                // column label                                                
                string xlabel;                                                 
                if (not xtitle.empty())                                        
                {                                                              
                    if (not xunit.empty())                                     
                    {                                                          
                        string format = "%" + xprecision + "f " + xunit + " $<$ %s $<$ %" + xprecision + "f " + xunit;
                        xlabel = Form(format.c_str(), xmin, xtitle.c_str(), xmax);
                    }                                                          
                    else                                                       
                    {                                                          
                        string format = "%" + xprecision + "f $<$ %s $<$ %" + xprecision + "f";
                        xlabel = Form(format.c_str(), xmin, xtitle.c_str(), xmax);
                    }                                                          
                }                                                              
                else                                                           
                {                                                              
                    string format = "%" + xprecision + "f $<$ %s $<$ %" + xprecision + "f";
                    xlabel = Form(format.c_str(), xmin, xaxis->GetTitle(), xmax);
                }                                                              
                table.setRowLabel(xlabel, row);                                
                                                                               
                // set the cell                                                
                string entry = lt::pm(hist->GetBinContent(xbin), hist->GetBinError(xbin), precision);
                table.setCell(entry, row, 0);                                  
            }                                                                  
        }                                                                      
                                                                               
        return table;                                                          
    }                                                                          

    // inflate the bin error by percentage of the bin value (used for systematic uncertainties)
    void SetSystematicUncertainty(TH1* const h, const float sys_unc)
    {
        for (int i = 0; i != h->GetNbinsX()+1; i++)
        {
            float v = h->GetBinContent(i);
            float e = h->GetBinError(i);
            float error = sqrt(e*e + sys_unc*sys_unc*v*v);
            h->SetBinError(i, error);
        }
        return;
    }

    // calculate the integral and error
    std::pair<double, double> IntegralAndError(TH1* const hist_ptr, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::IntegralAndError() -- hist pointer is NULL!");
        }
        int max_bin  = hist_ptr->GetNbinsX()+1;
        int min_bin  = 0;
        double error = 0;
        double integral = hist_ptr->IntegralAndError(min_bin, max_bin, error, option.c_str());
        return std::make_pair(integral, error);
    }

    // calculate the integral and error over a range
    std::pair<double, double> IntegralAndError(TH1* const hist_ptr, const float low, const float high, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::IntegralAndError] Error: hist pointer is NULL!");
        }
        if (low > high)
        {
            throw std::runtime_error("[rt::IntegralAndError] Error: low must > high!");
        }
        int left_bin  = hist_ptr->FindBin(low);
        int right_bin = hist_ptr->FindBin(high);
        if (lt::is_equal(static_cast<double>(high), hist_ptr->GetXaxis()->GetBinLowEdge(right_bin)))
        {
            right_bin--;
        }
        double error = 0;
        double integral = hist_ptr->IntegralAndError(left_bin, right_bin, error, option.c_str());
        return std::make_pair(integral, error);
    }

    // calculate the integral and error
    std::pair<double, double> EntriesAndError(TH1* const hist_ptr, const std::string& option)
    {
        if (!hist_ptr)
        {
            throw std::runtime_error("[rt::EntriesAndError] Error: hist pointer is NULL!");
        }
        double entries = hist_ptr->GetEntries(); 
        double error   = sqrt(entries); 
        return std::make_pair(entries, error);
    }

    // find a "bin" in a vector given a value
    unsigned int find_bin(const float value, const std::vector<float>& bins)
    {
        unsigned int index = 0;
        for (size_t i=0; i != bins.size()-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

    // find a "bin" in a vector given a value
    unsigned int find_bin(const double value, const std::vector<double>& bins)
    {
        unsigned int index = 0;
        for (size_t i=0; i != bins.size()-1; i++)
        {
            if (bins[i] < value && value < bins[i+1]) 
            {
                index=i;
                break;
            }
        }
        return index;
    }

} // namespace rt
