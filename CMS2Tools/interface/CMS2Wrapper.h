#ifndef CMS2WRAPPER_H
#define CMS2WRAPPER_H

// simple wrapper to cms2 to use with at::ScanChain()

class CMS2;

void Init(CMS2& cms2, TTree* tree);
void GetEntry(CMS2& cms2, long event);
void LoadAllBranches(CMS2& cms2);
bool IsRealData(CMS2& cms2);
unsigned int Run(CMS2& cms2);
unsigned int LumiBlock(CMS2& cms2);
unsigned int Event(CMS2& cms2);

#endif //#efine CMS2WRAPPER_H
