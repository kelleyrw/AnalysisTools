#include "AnalysisTools/CMS2Tools/interface/LoadFWLite.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "TString.h"
#include "TSystem.h"
#include <iostream>

namespace at
{
    void LoadFWLite()
    {
        // The CMSSW environment is defined (this is true even for FW Lite)
        // so set up the rest.
        const TString scram_arch = gSystem->Getenv("SCRAM_ARCH");
        const bool is_mac        = scram_arch.Contains("osx");

        std::cout << "Loading CMSSW FWLite." << std::endl;
        if (is_mac)
        {
            gSystem->Load("libFWCoreFWLite.dylib");
            AutoLibraryLoader::enable();
            gSystem->Load("libDataFormatsFWLite.dylib");
            gSystem->Load("libDataFormatsPatCandidates.dylib");
        }
        else
        {
            gSystem->Load("libFWCoreFWLite.so");
            AutoLibraryLoader::enable();
            gSystem->Load("libDataFormatsFWLite.so");
            gSystem->Load("libDataFormatsPatCandidates.so");
        }
    }

} //namespace at

