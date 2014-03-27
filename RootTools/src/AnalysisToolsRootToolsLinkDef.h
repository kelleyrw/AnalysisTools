#include "AnalysisTools/RootTools/interface/RootTools.h"
#ifdef __MAKECINT__ 

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace rt;

#pragma link C++ class rt::TH1Container;
#pragma link C++ class rt::TH1Overlay;

#pragma link C++ function rt::GetHistFromRootFile<TH1>;
#pragma link C++ function rt::GetHistFromRootFile<TH1D>;
#pragma link C++ function rt::GetHistFromRootFile<TH1F>;
#pragma link C++ function rt::GetHistFromRootFile<TH2>;
#pragma link C++ function rt::GetHistFromRootFile<TH2D>;
#pragma link C++ function rt::GetHistFromRootFile<TH2F>;
#pragma link C++ function rt::OpenRootFile;

#endif
