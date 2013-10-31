// C++
#include <stdexcept>
#include <iostream>
#include <algorithm>

// Root includes
#include "TFile.h"
#include "TCanvas.h"

// Tools
#include "AnalysisTools/LanguageTools/interface/LanguageTools.h"

// The function definitions for the templated classes from misc_tools.h

namespace rt
{
    namespace impl
    {

        template <typename T> T& source(T &object)
        {
            return object;
        }

        template <typename T> const T& source(const T &object)
        {
            return object;
        }

        template <typename T> T& source(T *object)
        {
            return *object;
        }

        template <typename T> const T& source(const T *object)
        {
            return *object;
        }

        // test for null
        template <typename T> bool test_source(const T&) {return true;}
        template <typename T> bool test_source(const T* ptr) {return ptr != NULL;}

    } // namespace impl

    // print the ROOT object to eps/png/pdf
    template <typename RootObjectType>
    void Print
    (
        RootObjectType obj,
        const std::string& file_name,
        const std::string& suffix,
        const std::string& option,
        const bool logy
    )
    {
        if(not(suffix == "eps" || suffix == "png" || suffix == "pdf" || suffix == "all"))
        {
            std::cout << "suffix " << suffix << " not valid!  No print." << std::endl;
            return;
        }

        TCanvas c1("c1_print_all_temp", "c1_print_all_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(lt::dirname(file_name), /*recursive=*/true);

        if (!impl::test_source(obj))
        {
            std::cout << "[rt::Print] Warning: Object is NULL -- exiting!" << std::endl;
            return;
        }
        impl::source(obj).Draw( option.c_str() );
        if (suffix != "all")
        {
            c1.Print( (file_name + "." + suffix).c_str() );
        }
        else
        {
            c1.Print((file_name + ".png").c_str());
            c1.Print((file_name + ".eps").c_str());
            c1.Print((file_name + ".pdf").c_str());
        }
        rt::CopyIndexPhp(lt::dirname(file_name));
        return;
    }

    // print the plots to eps/png/pdf
    template <typename RootObjectType>
    void Print
    (
        std::map<std::string, RootObjectType>& m,
        const std::string& dir_name,
        const std::string& suffix,
        const std::string& option,
        const bool logy
    )
    {
        if(not(suffix == "eps" || suffix == "png" || suffix == "pdf" || suffix == "all"))
        {
            std::cout << "suffix " << suffix << " not valid!  No print." << std::endl;
            return;
        }

        TCanvas c1("c1_print_all_temp", "c1_print_all_temp");
        c1.SetLogy(logy);
        c1.cd();

        lt::mkdir(dir_name, /*recursive=*/true);

        for(typename std::map<std::string,RootObjectType>::iterator itr = m.begin(); itr != m.end(); itr++)
        {
            if (!impl::test_source(itr->second))
            {
                std::cout << "[rt::Print] Warning: Object associated to " << itr->first << " is NULL -- skipping!" << std::endl;
                continue;
            }
            impl::source(itr->second).Draw( option.c_str() );
            if (suffix != "all")
            {
                c1.Print( (dir_name + "/" + itr->first + "." + suffix).c_str() );
            }
            else
            {
                c1.Print((dir_name + "/" + itr->first + ".png").c_str());
                c1.Print((dir_name + "/" + itr->first + ".pdf").c_str());
                c1.Print((dir_name + "/" + itr->first + ".eps").c_str());
            }
        }
        rt::CopyIndexPhp(dir_name);
        return;
    }

} // namespace rt
