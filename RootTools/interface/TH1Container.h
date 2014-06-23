#ifndef RT_TH1CONTAINER_H
#define RT_TH1CONTAINER_H

// c++ includes
#include <string>
#include <memory>

// ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TDirectory.h"

namespace rt
{

    class TH1Container
    {
        public:

            // constructors
            TH1Container();
            ~TH1Container();
            TH1Container(const std::string& file_name, const std::string& root_file_dir = "");
            TH1Container(const TH1Container& rhs);
            TH1Container& operator=(const TH1Container& rhs);
            TH1Container operator+(const TH1Container& rhs);
            TH1Container operator-(const TH1Container& rhs);
            TH1Container& operator+=(const TH1Container& rhs);
            TH1Container& operator-=(const TH1Container& rhs);
            void Swap(TH1Container& other);

            // add a histogram to the container 
            // (default is to skip duplicates, set overwite to write over)
            // WARNING: This take ownership of the hist pointer and will delete when the TH1Container goes out of scope! 
            void Add(TH1* hist_ptr, const bool overwrite = false);
            void Add(TH1* hist_ptr, const std::string& option, const bool overwrite = false);

            // add a histogram to the container 
            // (default is to skip duplicates, set overwite to write over)
            // These will make a clone of the histogram
            void Add(const TH1& hist_ptr, const bool overwrite = false);
            void Add(const TH1& hist_ptr, const std::string& option, const bool overwrite = false);

            // set the directory of the hists (needed for draw)
            void SetDirectory(TDirectory* dir = NULL);
            void SetDirectory(const std::string& hist_name, TDirectory* const dir = NULL);

            // remove a histogram
            void Remove(const std::string& hist_name);
            void Remove(const char* hist_name);
            void Remove(const TString& hist_name);
            void Remove(TH1* const hist_ptr);

            // return a histogram (throws if not found)
            TH1* Hist(const std::string& hist_name) const;
            TH1* Hist(const char* hist_name) const;
            TH1* Hist(const TString& hist_name) const;
            TH1* operator [] (const std::string& hist_name) const;
            TH1* operator [] (const char* hist_name) const;
            TH1* operator [] (const TString& hist_name) const;

            template <typename THType>
            THType& as(const std::string& hist_name) const {TH1* const h = Hist(hist_name); return *static_cast<THType*>(h);}

            // contains a hist?
            bool Contains(const std::string& hist_name) const;

            // load all the histograms from a root file
            void Load(const std::string& file_name, const std::string& root_file_dir = "");

            // clear all the histograms
            void Clear();

            // list all the hist in the container
            void List() const;

            // get a list of all the histograms
            std::vector<std::string> GetListOfHistograms() const;

            // a simple viewer function (only interactive version -- not ported over yet)
            //void View();

            // scale all the histograms in the container 
            void Scale(const double scale, const std::string& option = "");

            // normalize all the histograms in the container to the given value 
            void Normalize(const double value = 1.0);

            // call Sumw2() on each histogram
            void Sumw2();

            // histogram attributes (sets them all) 
            void SetLineColor(const Color_t color);
            void SetLineStyle(const Style_t style);
            void SetLineWidth(const Width_t width);
            void SetMarkerColor(const Color_t color = 1);
            void SetMarkerSize(const Size_t size = 1);
            void SetMarkerStyle(const Style_t style = 1);
            void SetOption(const std::string& option = "");
            void SetDrawOption(const std::string& option = "");
            void SetStats(const bool stats = true);
            void SetMinMax(const float min, const float max);

            // write all histograms to a root file
            void Write(const std::string& file_name, const std::string& root_file_dir = "", const std::string& option = "RECREATE") const;
            void Write(TFile* root_file, const std::string& root_file_dir = "") const;

            // print all histograms to an (eps, png, pdf)
            void Print(const std::string& dir_name, const std::string& suffix = "png", const std::string& option = "", bool logy = false) const;

            // static methods
            static void SetVerbose(const bool verbose = true);

        private:

            // data members
            struct impl;
            std::auto_ptr<impl> m_pimpl;
    };

} // namespace rt

#endif // RT_TH1CONTAINER_H
