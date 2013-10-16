#include "AnalysisTools/RootTools/interface/TH1Container.h"
#include "AnalysisTools/RootTools/interface/TH1Tools.h"
#include "AnalysisTools/RootTools/interface/MiscTools.h"
#include "AnalysisTools/LanguageTools/interface/OSTools.h"

// c++ includes
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

// Root includes
#include "TClass.h"
#include "TDirectory.h"
#include "TPad.h"
#include "TCanvas.h"

// boost includes
#include <boost/shared_ptr.hpp>

// other includes
// #include "rt/MiscTools.h"
// #include "rt/Keyboard.h"

// using std
using namespace std;

// smart pointer for histograms
typedef boost::shared_ptr<TH1> TH1Ptr;

namespace rt
{

    // declare data struct
    // ---------------------------------------------------------------------------------------- //

    struct TH1Container::impl
    {
        static bool verbose;
        map<string, TH1Ptr> hist_map;
    };


    // constructor
    // ---------------------------------------------------------------------------------------- //

    TH1Container::TH1Container()
        : m_pimpl(new TH1Container::impl)
    {
    }

    TH1Container::TH1Container(const std::string& file_name, const std::string& root_file_dir)
        : m_pimpl(new TH1Container::impl)
    {
        Load(file_name, root_file_dir);
    }

    TH1Container::~TH1Container()
    {
    }

    TH1Container::TH1Container(const TH1Container& rhs)
        : m_pimpl(new TH1Container::impl)
    {
        for (map<string, TH1Ptr>::const_iterator iter = rhs.m_pimpl->hist_map.begin(); iter != rhs.m_pimpl->hist_map.end(); iter++)
        {
            m_pimpl->hist_map[iter->first] = TH1Ptr(dynamic_cast<TH1*>(iter->second->Clone()));   
        }
        return;
    }

    void TH1Container::Swap(TH1Container& rhs)
    {
        std::swap(m_pimpl->verbose , rhs.m_pimpl->verbose );
        std::swap(m_pimpl->hist_map, rhs.m_pimpl->hist_map);
        return;
    }

    TH1Container& TH1Container::operator=(const TH1Container& rhs)
    {
        TH1Container temp(rhs);
        Swap(temp);
        return *this;
    }


    // members
    // ---------------------------------------------------------------------------------------- //

    TH1Container TH1Container::operator+(const TH1Container& rhs)
    {
        TH1Container temp(*this);
        for (map<string, TH1Ptr>::const_iterator iter = rhs.m_pimpl->hist_map.begin(); iter != rhs.m_pimpl->hist_map.end(); iter++)
        {
            if (temp.Contains(iter->first))
            {
                temp[iter->first]->Add(iter->second.get());
            }
            else
            {
                temp.Add(iter->second.get());
            }
        }
        return temp;
    }

    TH1Container& TH1Container::operator+=(const TH1Container& rhs)
    {
        TH1Container temp = this->operator+(rhs);
        Swap(temp);
        return *this;
    }

    void TH1Container::Add(TH1* hist_ptr, const bool overwrite)
    {
        // do we already have the histogram?
        string name = hist_ptr->GetName();
        bool hist_name_used = m_pimpl->hist_map.end() != m_pimpl->hist_map.find(name);
        if (hist_name_used)
        {
            try
            {
                bool same_pointer = (hist_ptr == m_pimpl->hist_map.at(name).get());
                if (overwrite && !same_pointer)
                {
                    cout << "[TH1Container::add()] Warning: '" << name << "' already exists.  Overwriting!" << endl;
                }
                else if (overwrite && same_pointer)
                {
                    cout << "[TH1Container::add()] Warning: '" << name << "' already exists.  Leaving it alone!" << endl;
                    return;
                }
                else
                {
                    cout << "[TH1Container::add()] Warning: '" << name << "' already exists.  Skipping!" << endl;
                    return;
                }
            }
            catch(exception& e)
            {
                cout << "[rt::TH1Container::add()] Error " << e.what() << endl;
            }
        }
        if (m_pimpl->verbose)
        {
            cout << "[TH1Container::add()] Adding " << name << endl;
        }

        // I want to be in charge of deleting
        hist_ptr->SetDirectory(0);
        m_pimpl->hist_map.insert(pair<string, TH1Ptr>(name, TH1Ptr(hist_ptr)));
        return;
    }

    void TH1Container::Add(TH1* hist_ptr, const std::string& option, const bool overwrite)
    {
        hist_ptr->SetOption(option.c_str());
        Add(hist_ptr, overwrite);
    }

    void TH1Container::Add(const TH1& hist, const bool overwrite)
    {
        Add(dynamic_cast<TH1*>(hist.Clone()), overwrite);
    }

    void TH1Container::Add(const TH1& hist, const std::string& option, const bool overwrite)
    {
        Add(dynamic_cast<TH1*>(hist.Clone()), option, overwrite);
    }

    // set the directory of the hists (needed for draw)
    void TH1Container::SetDirectory(TDirectory* const dir)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetDirectory(dir);
        }
    }

    void TH1Container::SetDirectory(const std::string& hist_name, TDirectory* const dir)
    {
        Hist(hist_name)->SetDirectory(dir);
    }


    // remove a histogram
    void TH1Container::Remove(const string& hist_name)
    {
        m_pimpl->hist_map.erase(hist_name);
    }

    void TH1Container::Remove(const char* hist_name)
    {
        Remove(static_cast<string>(hist_name));
    }

    void TH1Container::Remove(const TString& hist_name)
    {
        Remove(static_cast<string>(hist_name.Data()));
    }

    void TH1Container::Remove(TH1* const hist_ptr)
    {
        Remove(string(hist_ptr->GetName()));
    }

    // return a histogram (throws if not found)
    TH1* TH1Container::Hist(const std::string& hist_name) const
    {
        try
        {
            return m_pimpl->hist_map.at(hist_name).get();
        }
        catch(exception& e)
        {
            throw(std::domain_error("[rt::TH1Container::hist()] Error: " + hist_name + " not found!  Aborting."));
        }
    }


    TH1* TH1Container::Hist(const char* hist_name) const
    {
        return Hist(static_cast<string>(hist_name));
    }


    TH1* TH1Container::Hist(const TString& hist_name) const
    {
        return Hist(static_cast<string>(hist_name.Data()));
    }


    TH1* TH1Container::operator [] (const std::string& hist_name) const
    {
        return Hist(hist_name);
    }


    TH1* TH1Container::operator [] (const char* hist_name) const
    {
        return Hist(static_cast<string>(hist_name));
    }


    TH1* TH1Container::operator [] (const TString& hist_name) const
    {
        return Hist(static_cast<string>(hist_name.Data()));
    }

    bool TH1Container::Contains(const std::string& hist_name) const
    {
        return m_pimpl->hist_map.find(hist_name) != m_pimpl->hist_map.end();
    }

    void TH1Container::Clear()
    {
        m_pimpl->hist_map.clear();
        return;
    }

    void TH1Container::Load(const std::string& file_name, const std::string& root_file_dir)
    {
        using namespace std;

        // file load
        auto_ptr<TFile> file(new TFile(file_name.c_str()));
        if (file->IsZombie()) 
        {
            throw runtime_error(string("[rt::TH1Container::Load()] Error: '") + file->GetName() + "' not found or empty.  Aborting!");
        }
        if (!file->cd(root_file_dir.c_str()))
        {
            cout << "[rt::TH1Container::Load()] Warning: '" << root_file_dir 
                << " is not in the ROOT file (" << file->GetName() 
                << ").  Doing nothing." << endl;
            return;
        }

        // get the hists
        for (TObjLink* link = gDirectory->GetListOfKeys()->FirstLink(); link != NULL; link = link->Next())
        {
            if (TH1* hist_ptr = dynamic_cast<TH1*>(gDirectory->Get(link->GetObject()->GetName())))
            {
                // I want to be in charge of deleting
                hist_ptr->SetDirectory(0);
                string name(hist_ptr->GetName());
                m_pimpl->hist_map.insert(pair<string, TH1Ptr>(name, TH1Ptr(hist_ptr)));
            }
        }
        file->Close();
        return;
    }

    void TH1Container::List() const
    {
        cout << "[TH1Container::List()]: listing all histograms in the container" << endl;
        cout <<  "  " << setw(15) << left << "Hist Type" << "\t" << setw(15) << left << "Hist Name" << "\t" << "Hist Title" << endl; 
        cout <<  "-----------------------------------------------------------------------" << endl; 
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            cout <<  "  " << setw(15) << left << iter->second->ClassName() << "\t" << setw(15) << left << iter->first 
                << "\t" << iter->second->GetTitle() << endl;
        }
    }

    //void usage()
    //{
    //    cout << "TH1Container::view() usage:" << endl;
    //    cout << "up   --> draw next histogram" << endl;
    //    cout << "down --> draw previous histogram" << endl;
    //    cout << "q    --> quit the viewer" << endl;
    //}

    //void draw(TH1* h)
    //{
    //    if (h)
    //    {
    //        h->Draw();
    //        gPad->Update();
    //        cout << "drawing:\t" << h->GetName() << endl;
    //    }
    //    else
    //    {
    //        cout << "histogram is NULL" << endl;
    //    }
    //}

    // viewing the histograms
    //void TH1Container::View()
    //{
    //    usage();
    //    map<string, TH1Ptr>::const_iterator begin_iter = m_pimpl->hist_map.begin();
    //    map<string, TH1Ptr>::const_iterator end_iter   = m_pimpl->hist_map.end();
    //    map<string, TH1Ptr>::const_iterator iter       = begin_iter;

    //    // draw the first one
    //    draw(iter->second);

    //    rt::key::value_type input = rt::key::null;
    //    while(input != rt::key::q)
    //    {
    //        input = rt::get_character_from_keyboard();
    //        // process input
    //        switch(input)
    //        {
    //            case rt::key::up: 
    //                iter++; 
    //                if (iter == end_iter)
    //                {
    //                    iter = begin_iter;
    //                }
    //                break;
    //            case rt::key::down: 
    //                if (iter == begin_iter)
    //                {
    //                    iter = end_iter;
    //                }
    //                iter--; 
    //                break;
    //            case rt::key::q: 
    //                cout << "quiting TH1Container::view()" << endl;
    //                continue;
    //                break;
    //            default: usage(); continue;
    //        }

    //        // draw the current one 
    //        draw(iter->second);
    //    }
    //}

    void TH1Container::Scale(const double scale, const std::string& option)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->Scale(scale, option.c_str());
        }
    }

    void TH1Container::Normalize(const double value)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            rt::Normalize(iter->second.get(), value);
        }
    }

    void TH1Container::Sumw2()
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            if (!iter->second->GetSumw2N())
            {
                iter->second->Sumw2();
            }
        }
    }

    void TH1Container::SetLineColor(const Color_t color)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetLineColor(color);
        }
    }

    void TH1Container::SetLineStyle(const Style_t style)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetLineStyle(style);
        }
    }

    void TH1Container::SetLineWidth(const Width_t width)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetLineWidth(width);
        }
    }

    void TH1Container::SetMarkerColor(const Color_t color)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetMarkerColor(color);
        }
    }

    void TH1Container::SetMarkerSize(const Size_t size)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetMarkerSize(size);
        }
    }

    void TH1Container::SetMarkerStyle(const Style_t style)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetMarkerStyle(style);
        }
    }

    void TH1Container::SetOption(const std::string& option)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetOption(option.c_str());
        }
    }

    void TH1Container::SetDrawOption(const std::string& option)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetDrawOption(option.c_str());
        }
    }

    void TH1Container::SetStats(const bool stats)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetStats(stats);
        }
    }

    void TH1Container::SetMinMax(const float min, const float max)
    {
        for (map<string, TH1Ptr>::const_iterator iter = m_pimpl->hist_map.begin(); iter != m_pimpl->hist_map.end(); iter++)
        {
            iter->second->SetMinimum(min);
            iter->second->SetMaximum(max);
        }
    }

    void TH1Container::Write(const std::string& file_name, const std::string& root_file_dir, const std::string& option) const
    {
        lt::mkdir(lt::dirname(file_name), /*recursive=*/true);
        TFile* output_file = TFile::Open(file_name.c_str(), option.c_str());
        Write(output_file, root_file_dir);
        return;
    }

    void TH1Container::Write(TFile* root_file, const std::string& root_file_dir) const
    {
        root_file->cd("");
        if (!root_file->cd(root_file_dir.c_str()))
        {
            root_file->mkdir(root_file_dir.c_str());
        }
        root_file->cd(root_file_dir.c_str());

        for (std::map<std::string, boost::shared_ptr<TH1> >::iterator itr = m_pimpl->hist_map.begin(); itr != m_pimpl->hist_map.end(); itr++)
        {
            itr->second->Write(itr->first.c_str(), TObject::kOverwrite);
        }
        root_file->Close();
    }

    void TH1Container::Print(const std::string& dir_name, const std::string& suffix, const std::string& option, bool logy) const
    {
        //rt::mkdir(dir_name, /*force=*/true);
        //rt::Print(m_pimpl->hist_map, dir_name, suffix, option, logy);
        if (not (suffix == "eps" or suffix == "png" or suffix == "pdf" ))
        {
            cout << "suffix " << suffix << " not valid!  No print." << endl;
            return;
        }

        TCanvas c1("c1_Print_temp", "c1_Print_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(dir_name, /*recursive=*/true);

        for (std::map<std::string, boost::shared_ptr<TH1> >::iterator itr = m_pimpl->hist_map.begin(); itr != m_pimpl->hist_map.end(); itr++)
        {
            if (!itr->second)
            {
                cout << "rt::Print() Warning: Object associated to " 
                    << itr->first << " is NULL -- skipping!" << endl;
                continue;
            }
            itr->second->Draw(option.c_str());
            c1.Print((dir_name + "/" + itr->first + "." + suffix).c_str());
        }
        rt::CopyIndexPhp(dir_name);
        return;
    }

    // verbosity setting
    bool TH1Container::impl::verbose = false;
    void TH1Container::SetVerbose(const bool verbose)
    {
        TH1Container::impl::verbose = verbose;        
    }

} // namespace rt

