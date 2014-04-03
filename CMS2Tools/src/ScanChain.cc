#include "AnalysisTools/CMS2Tools/interface/ScanChain.h"

// c++
#include <iostream>
#include <stdexcept>
#include <set>

// ROOT
#include "TChain.h"
#include "TTreeCache.h"
#include "TBenchmark.h"

// CMS2
#include "CMS2/NtupleMacrosHeader/interface/CMS2.h"

namespace at
{
    // a simple function to test ScanChain
    int ScanChainTestAnalysis(long event)
    {
        std::cout << "run = " <<  tas::evt_run() << "\tevent = " << tas::evt_event() << std::endl;
        return 0;
    }

} // namespace at

