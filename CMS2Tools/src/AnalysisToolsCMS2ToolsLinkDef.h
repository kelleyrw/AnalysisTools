//------------------------------------------------------------------------//
// Analysis/CMS2Tools LinkDef.h Kelley 2011-05-01
//------------------------------------------------------------------------//

#include "AnalysisTools/CMS2Tools/interface/LoadFWLite.h"
#include "AnalysisTools/CMS2Tools/interface/DileptonChargeType.h"
#include "AnalysisTools/CMS2Tools/interface/DileptonHypType.h"

#ifdef __CINT__  // interpreter and ACLiC

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions; 

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

// namespaces
#pragma link C++ namespace at;

// typedefs/enums
#pragma link C++ enum at::DileptonHypType::value_type;
#pragma link C++ enum at::DileptonChargeType::value_type;

// templated classes

// classes or structs
#pragma link C++ struct at::DileptonHypType;
#pragma link C++ struct at::DileptonChargeType;

// functions
#pragma link C++ function at::LoadFWLite;

#endif // __CINT__
