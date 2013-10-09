#ifndef RT_TH1TOOLS_H
#define RT_TH1TOOLS_H

// helper and functions for ROOT TH1 related operations (uses ROOT name conventions)
// -------------------------------------------------------------------------------------------------//

// c++ includes
#include <string>
#include <map>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"

// Ian's table class
#include "AnalysisTools/LanguageTools/interface/CTable.h"

// namespace rt --> root tools
namespace rt
{
    // get a root file (throws if file not found)
    TFile* OpenRootFile(const std::string& file_name, const std::string& option = "");

    // get a single histgram from a root file (client is the owner)
    template <typename TH1Type>
    TH1Type* GetHistFromRootFile(const std::string& file_name, const std::string& hist_name);
    
    // get all the hists from a root file (given a file) in a map of hist pointers 
    std::map<std::string, TH1*> GetMapOfTH1s(TFile* const file, const std::string& root_file_dir = "");
    
    // get all the hists from a root file (given a filename) in a map of hist pointers 
    std::map<std::string, TH1*> GetMapOfTH1s(const std::string& filename, const std::string& root_file_dir = "");
    
    // get all the hists from a root file (given a file) in a vector of hist pointers 
    std::vector<TH1*> GetVectorOfTH1s(TFile* const file, const std::string& root_file_dir = "");
    
    // get all the hists from a root file (given a filename) in a vector of hist pointers 
    std::vector<TH1*> GetVectorOfTH1s(const std::string& filename, const std::string& root_file_dir = "");

   // save the contents of the map to a root file
   void Write
   (
        std::map<std::string, TH1*>& hist_map, 
        const std::string& file_name, 
        const std::string& root_file_dir = "",
        const std::string& option = "UPDATE"
   );

   void Write
   (
        std::map<std::string, TH1*>& hist_map, 
        TFile* root_file,
        const std::string& root_file_dir = ""
   );

   // save the contents of the vector to a root file
   void Write
   (
        std::vector<TH1*>& hist_vector, 
        const std::string& file_name, 
        const std::string& root_file_dir = "",
        const std::string& option = "UPDATE"
   );

   // save a hist to a ROOT file 
   void Write
   (
        const TH1* const hist_ptr, 
        const std::string& file_name, 
        const std::string& root_file_dir = "",
        const std::string& option = "UPDATE"
   );

//    // print the histogram to eps/png/pdt
//    void Print
//    (
//         const TH1* const hist_ptr, 
//         const std::string& dir_name, 
//         const std::string& suffix = "png", 
//         const std::string& file_name = "", 
//         const std::string& option = "",
//         const bool logy = false
//    );
//
//    // print the plots to eps/png/pdf 
//    void Print
//    (
//         std::map<std::string, TH1*>& hist_map, 
//         const std::string& dir_name, 
//         const std::string& suffix = "png",
//         const std::string& option = "",
//         bool logy = false
//    );
//
//    // print the plots to eps/png/pdf 
//    void Print
//    (
//         std::vector<TH1*>& hist_vector, 
//         const std::string& dir_name, 
//         const std::string& suffix = "png",
//         const std::string& option = "",
//         bool logy = false
//    );
    
    // make an efficiency plot by dividing the two histograms 
    TH1* MakeEfficiencyPlot
    (
        TH1* const num_hist, 
        TH1* const den_hist, 
        const std::string& name = "h_eff", 
        const std::string& title = ""
    );
    
    // make a 2D efficiency plot by dividing the two histograms 
    TH2* MakeEfficiencyPlot2D
    (
        TH1* const num_hist, 
        TH1* const den_hist, 
        const std::string& name = "h_eff2D",
        const std::string& title = ""
    );
    
    TH2* MakeEfficiencyPlot2D
    (
        TH2* const num_hist, 
        TH2* const den_hist, 
        const std::string& name = "h_eff2D",
        const std::string& title = ""
    );
    
    // create a Projection from 2D hist 
    TH1* MakeProjectionPlot
    (
        TH2* const hist, 
        const std::string& axis = "x", 
        const std::string& name = "", 
        const std::string& title = "", 
        double low = 0.0, 
        double high = -1.0,
        const std::string& option = ""
    );
    
    // create a Projection from num and den hists 
    TH1* MakeEfficiencyProjectionPlot
    (
        TH2* const num_hist, 
        TH2* const den_hist, 
        const std::string& axis = "x", 
        const std::string& name = "", 
        const std::string& title = "", 
        double low = 0.0, 
        double high = -1.0,
        const std::string& option = ""
    );
    
    // create a Projection from num and den hists 
    // this version for convenience (num and den hist must be castable to TH2*)
    TH1* MakeEfficiencyProjectionPlot
    (
        TH1* const num_hist,  
        TH1* const den_hist, 
        const std::string& axis = "x", 
        const std::string& name = "", 
        const std::string& title = "", 
        double low = 0.0, 
        double high = -1.0,
        const std::string& option = ""
    );
    
    // add Hists and return new hist (client is owner of the TH1*)
    TH1* AddHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title = "");
    TH1* AddHists(TH1* const h1, TH1* const h2, TH1* const h3, const std::string& name, const std::string& title = "");
    
    // subtract Hists and return new hist (client is owner of the TH1*)
    TH1* SubtractHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title = "");
    
    // divide Hists and return new hist (client is owner of the TH1*)
    TH1* DivideHists(TH1* const h_num, TH1* const h_den, const std::string& name, const std::string& title = "", const std::string& option = "");
    
    // multiply Hists and return new hist (client is owner of the TH1*)
    TH1* MultiplyHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title = "", const std::string& option = "");
    
    // subtract Hists and then divide by the first and return new hist (client is owner of the TH1*)
    // rel diff = (h1 - h2)/h1
    TH1* RelativeDiffHists(TH1* const h1, TH1* const h2, const std::string& name, const std::string& title = "");
    
    // mask off all values that are in the range and set the values to zero 
    void MaskHist2D(TH1* const h1, std::string axis_label, const float low, const float high);
    
    // maximum value in hist 
    double GetHistMaximumValue(TH1* const hist, bool include_error = false);
    
    // minimum value in hist 
    double GetHistMinimumValue(TH1* const hist, bool include_error = false);
    
    // scale the histogram to be normalized to scale 
    void Normalize(TH1* const hist, const double scale = 1.0);
    
    // calculate the integral 1D
    double Integral(TH1* const h, const std::string& option = "");
    double Integral(TH1* const h, const float low, const float high, const std::string& option = "");
    
    // calculate the integral 2D
    double Integral(TH2* const h, const std::string& option = "");
    double Integral(TH2* const h, const float xlow, const float xhigh, const float ylow, const float yhigh, const std::string& option = "");
    
    // get the bin content with float values
    double GetBinContent1D(TH1* const h, const float x);
    double GetBinContent2D(TH2* const h, const float x, const float y);
    
    // get the bin error with float values
    double GetBinError1D(TH1* const h, const float x);
    double GetBinError2D(TH2* const h, const float x, const float y);
    
    // set the bin content with float values
    void SetBinContent1D(TH1* h, const float x, const float value);
    void SetBinContent1D(TH1* h, const float x, const float value, const float error);
    void SetBinContent2D(TH1* h, const float x, const float y, const float value);
    void SetBinContent2D(TH1* h, const float x, const float y, const float value, const float error);
    void SetBinContent2D(TH2* h, const float x, const float y, const float value);
    void SetBinContent2D(TH2* h, const float x, const float y, const float value, const float error);
    
     // calculate integral{low, high} of histogram / total integral of histogram
     double CalcPercentOfIntegral(TH1* const h, const float low, const float high, const std::string& option = "");
    
    // set statbox position
    void SetStatBoxPosition(TH1* const hist_ptr, const float x1 = 0.8, const float y1 = 0.8, const float x2 = 1.0, const float y2 = 1.0);
    
    // a fix to the issue of the erros on the profile hists
    void CalcErrorsForProfile(TProfile* const profile);
    
    // create a CTable from a hist (1D and 2D only)                            
     CTable CreateTableFromHist                                                 
     (                                                                          
         TH1* const hist,                                                             
         const std::string& title = "",                                         
         const std::string& xtitle = "",                                        
         const std::string& ytitle = "",                                        
         const std::string& xunit = "",                                         
         const std::string& yunit = "",                                         
         const std::string& precision = "1.2",                                  
         const std::string& xprecision = "1.2",                                 
         const std::string& yprecision = "1.2",                                 
         const bool reverse_axis = false                                        
     );                                                                         
     
    // inflate the bin error by percentage of the bin value (used for systematic uncertainties)
    void SetSystematicUncertainty(TH1* const h, const float sys_unc);
    
    // calculate the integral and Error
    std::pair<double, double> IntegralAndError(TH1* const h, const std::string& option = "");
    std::pair<double, double> IntegralAndError(TH1* const h, const float low, const float high, const std::string& option = "");
    
    // calculate the integral and Error
    std::pair<double, double> EntriesAndError(TH1* const h, const std::string& option = "");
    
    // helper for low level objects 
    // -------------------------------------------------------------------------------------------------//
    
    // only works on compile time arrays (e.g. float bins[] = {1,2,3};)
    template <int N> unsigned int find_bin(const float value, const float (&bins)[N]);
    template <int N> unsigned int find_bin(const double value, const double (&bins)[N]);

} // namespace rt

// definitions of templated functions
#include "TH1Tools.impl.h"

#endif // RT_TH1TOOLS_H
